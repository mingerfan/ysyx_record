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

bool npc_init_done = false;

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
  npc_init_done = true;
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
#ifdef CONFIG_NPC_WAVE
  tfp->dump(contextp->time());
  contextp->timeInc(1);
#endif
  dut.clock = 1;
  dut.eval();
#ifdef CONFIG_NPC_WAVE
  tfp->dump(contextp->time());
#ifdef CONFIG_WAVE_RINGBUF
  static int cnt = 0;
  tfp->flush();
  if (cnt++ >= CONFIG_WAVE_RINGBUF_SIZE) {
    tfp->close();
    tfp->open("/home/xs/ysyx/ysyx-workbench/npc/build/sim.vcd");
    cnt = 0;
  }
#endif
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
  uint64_t exec_last_pc = dut.io_pc;
  for (;n > 0; n--) {
    itrace_recorde();
    extern bool use_difftest;
    if (use_ftrace) trace_ftrace_trace((uint32_t)paddr_read(exec_last_pc, 4), exec_last_pc);
    if (use_difftest) difftest_check(exec_last_pc); // 这里用last是为了方便debug，实际上状态确实是在当前pc不匹配的
    exec_last_pc = dut.io_pc;
    npc_eval();
    if (use_difftest) {
      difftest_step_nocheck(dut.io_pc, 0);
    }
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
  extern bool use_ftrace;
  if (use_ftrace) {
    trace_ftrace_print();
  }
  return state_check_return();
}

#endif