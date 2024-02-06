#include <difftest.h>
#include <debug.h>
#include <dlfcn.h>
#include <state.h>
#include <macro.h>


void (*ref_difftest_memcpy)(paddr_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;

static bool is_skip_ref = false;
static int skip_dut_nr_inst = 0;
bool detach_difftest = false;

#ifdef CONFIG_DIFFTEST

void init_difftest(char *ref_so_file, long img_size, int port, CPU_state *cpu) 
{
    assert(ref_so_file != nullptr);

    void *handle;
    handle = dlopen(ref_so_file, RTLD_LAZY);
    assert(handle);

    ref_difftest_memcpy = (void(*)(paddr_t, void*, size_t, bool))dlsym(handle, "difftest_memcpy");
    assert(ref_difftest_memcpy);

    ref_difftest_regcpy = (void(*)(void*, bool))dlsym(handle, "difftest_regcpy");
    assert(ref_difftest_regcpy);

    ref_difftest_exec = (void(*)(uint64_t))dlsym(handle, "difftest_exec");
    assert(ref_difftest_exec);

    void (*ref_difftest_init)(int) = (void(*)(int))dlsym(handle, "difftest_init");
    assert(ref_difftest_init);

    ref_difftest_init(port);
    ref_difftest_memcpy(CONFIG_NPC_PC, guest_to_host(CONFIG_NPC_PC), img_size, DIFFTEST_TO_REF);
    ref_difftest_regcpy(cpu, DIFFTEST_TO_REF);

    G_DEBUG_I("DIFFTEST INIT COMPLETED");
}

void difftest_state_sync(CPU_state *cpu)
{
  ref_difftest_memcpy(CONFIG_NPC_PC, guest_to_host(CONFIG_NPC_PC), CONFIG_NPC_MSIZE, DIFFTEST_TO_REF);
  ref_difftest_regcpy(cpu, DIFFTEST_TO_REF);
}

// 在添加流水线以后，difftest的pc获取可能是一个问题，就得好好考虑一下怎么获取pc了
// 而且流水线不是一定每个时钟周期都更新pc，这也是一个问题
bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc)
{
    for (int i = 0; i < 32; i++) {
      // G_DEBUG_WR("name:%s dut:0x%016lx ref:0x%016lx\n", 
      // reg_name(i), gpr(i), ref_r->gpr[i]);
      if (gpr(i) != ref_r->gpr[i]) {
          G_DEBUG_WR("unmatched reg[%d]:%s DUT: 0x%016lx REF: 0x%016lx At 0x%lx\n", \
              i,   reg_name(i), gpr(i), ref_r->gpr[i], pc);
          return false;
      }
    }
    uint64_t data;
    #define check(csr) do { if (ref_r->csrs.csr != (data = get_csrs_byname(#csr, NULL))) {\
        G_DEBUG_WR("unmatched csr: %s DUT: 0x%016lx REF: 0x%016lx At 0x%08lx\n", \
            #csr, data, ref_r->csrs.csr, pc);\
            return false;\
    }} while(0);
    check(mepc);
    check(mcause);
    check(mtvec);
    check(mstatus);
    return true;
    #undef check
}

static void checkregs(CPU_state *ref, vaddr_t pc) 
{
  if (!isa_difftest_checkregs(ref, pc)) {
    npc_state.state = NPC_ABORT;
    npc_state.halt_pc = pc;
    rr_reg_display();
  }
}

void difftest_check(vaddr_t pc)
{
  CPU_state ref_r;
  if (!detach_difftest) {
    ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);
    checkregs(&ref_r, pc);
  }
}

// this is used to let ref skip instructions which
// can not produce consistent behavior with NEMU
void difftest_skip_ref() 
{
  is_skip_ref = true;
  // If such an instruction is one of the instruction packing in QEMU
  // (see below), we end the process of catching up with QEMU's pc to
  // keep the consistent behavior in our best.
  // Note that this is still not perfect: if the packed instructions
  // already write some memory, and the incoming instruction in NEMU
  // will load that memory, we will encounter false negative. But such
  // situation is infrequent.
  skip_dut_nr_inst = 0;
}

void difftest_step(vaddr_t pc, vaddr_t npc) 
{
  if (!detach_difftest) {
    CPU_state ref_r;

    if (skip_dut_nr_inst > 0) {
      ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);
      if (ref_r.pc == npc) {
        skip_dut_nr_inst = 0;
        checkregs(&ref_r, npc);
        return;
      }
      skip_dut_nr_inst --;
      if (skip_dut_nr_inst == 0)
        panic("can not catch up with ref.pc = " "0x%016lx" " at pc = " "0x%016lx", ref_r.pc, pc);
      return;
    }

    if (is_skip_ref) {
      // to skip the checking of an instruction, just copy the reg state to reference design
      CPU_state cpu;
      rr_wrap(&cpu);
      ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
      is_skip_ref = false;
      return;
    }

    ref_difftest_exec(1);
    ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);

    checkregs(&ref_r, npc);
  }
}

void difftest_step_nocheck(vaddr_t pc, vaddr_t npc) 
{
  if (!detach_difftest) {
    CPU_state ref_r;

    if (skip_dut_nr_inst > 0) {
      ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);
      if (ref_r.pc == pc) {
        skip_dut_nr_inst = 0;
        checkregs(&ref_r, pc);
        return;
      }
      skip_dut_nr_inst --;
      if (skip_dut_nr_inst == 0)
        panic("can not catch up with ref.pc = " "0x%016lx" " at pc = " "0x%016lx", ref_r.pc, pc);
      return;
    }

    if (is_skip_ref) {
      // to skip the checking of an instruction, just copy the reg state to reference design
      CPU_state cpu;
      rr_wrap(&cpu);
      ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
      is_skip_ref = false;
      return;
    }

    ref_difftest_exec(1);
  }
}

#endif