#include "generated/autoconf.h"
#include <Vtop.h>
#include "npc_memory.h"
#include "verilated_vcd_c.h"
#include "Vtop__Dpi.h"
#include "debug.h"
#include "monitor.h"
#include "state.h"
#include "regs.h"
#include "sdb.h"
#include "mytrace.h"
#include "difftest.h"

TOP_NAME dut;
static VerilatedVcdC *tfp = nullptr;
static VerilatedContext *contextp;

svScope scope = nullptr;

static void single_cycle() {
  dut.clock = 0; dut.eval();
  dut.clock = 1; dut.eval();
}

static void reset(int n) {
  dut.reset = 1;
  while (n -- > 0) single_cycle();
  dut.reset = 0;
}

static void init(int argc, char *argv[]) {
#ifdef CONFIG_NPC_WAVE
  Verilated::traceEverOn(true);
#endif
  contextp = new VerilatedContext;
  reset(10);
#ifdef CONFIG_NPC_WAVE
  tfp = new VerilatedVcdC;
  dut.trace(tfp, 99);
  tfp->open("/home/xs/ysyx/ysyx-workbench/npc/build/sim.vcd");
#endif
  monitor_init(argc, argv);
  npc_state.state = NPC_RUNNING;
  G_DEBUG_WR("Initialization Completed!\n");
}

static void npc_eval() {
#ifdef CONFIG_NPC_WAVE
  contextp->timeInc(1);
#endif
  dut.clock = 0;
  G_DEBUG_I("start read mem: %lx", dut.io_pc);
  dut.io_inst = (uint32_t)paddr_read(dut.io_pc, 4);
  G_DEBUG_I("addr: %lx inst:%x", dut.io_pc, (uint32_t)paddr_read(dut.io_pc, 4));
  dut.eval();
  if (npc_state.state != NPC_RUNNING) return;
#ifdef CONFIG_NPC_WAVE
  tfp->dump(contextp->time());
  contextp->timeInc(1);
#endif
  dut.clock = 1;
  dut.eval();
#ifdef CONFIG_NPC_WAVE
  tfp->dump(contextp->time());
#endif
  // G_DEBUG_I("is_ebreak:%d", is_ebreak());
}

static void de_init()
{
#ifdef CONFIG_NPC_WAVE
  tfp->close();
#endif
}

void cpu_exec(uint64_t n)
{
  switch (npc_state.state) {
    case NPC_END: case NPC_ABORT:
      printf("Program execution has ended. To restart the program, exit NPC and run again.\n");
      return;
    default: npc_state.state = NPC_RUNNING;
  }
  extern bool use_ftrace;
  for (;n > 0; n--) {
    itrace_recorde();
    if (use_ftrace) trace_ftrace_print((uint32_t)paddr_read(dut.io_pc, 4), dut.io_pc);
    npc_eval();
    // difftest_step(0, dut.io_pc);
    if (npc_state.state != NPC_RUNNING) {
      trace_inst_print(0);
      break;
    }
#ifdef CONFIG_WATCHPOINT
      WP* p;
      p = scan_wp();
      if (p) {
        npc_state.state = NPC_STOP;
        printf("WATCHPOINT Triggered!\n");
        trace_inst_print(1);
        info_single_wp(p);
        break;
      }
#endif

  }
  state_check();
}


#ifdef CONFIG_NPC

int main(int argc, char *argv[]) {
  init(argc, argv);
  sdb_mainloop();
  de_init();
  return 0;
}

#endif