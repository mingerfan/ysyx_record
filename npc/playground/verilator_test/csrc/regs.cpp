#include <regs.h>
#include <string.h>
#include <macro.h>


const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void rr_reg_display() 
{
  int i = 0;
  for (i = 0; i < 32; ++i) {
    G_DEBUG_WR("%3s\t=\t0x%08lx\n", reg_name(i), gpr(i));
  }
}

uint64_t rr_reg_str2val(const char *s, bool *success)
{
    #define USE_SUC(v) \
    do{ \
        if (success != NULL) {\
            *success = v; \
        }\
    } while(0)

    USE_SUC(false);
    if (strcmp("pc", s) == 0) {
        USE_SUC(true);
        return dut.io_pc;
    }
    for (int i = 0; i < ARRLEN(regs); ++i) {
        if (strcmp(regs[i], s) == 0) {
            USE_SUC(true);
            return gpr(i);
        }
    }
    return 0;
}

void rr_wrap(CPU_state *cpu)
{
  for (int i = 0; i < 32; i++) {
    cpu->gpr[i] = gpr(i);
    G_DEBUG_I("rr_wrap i:%d val:0x%016lx", i, cpu->gpr[i]);
  }
  cpu->pc = get_pc();
  G_DEBUG_I("rr_wrap pc_val:0x%016lx", cpu->pc);
}
