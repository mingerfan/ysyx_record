#include <regs.h>
#include <string.h>
#include <macro.h>


const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

const char *reg_csrs[] = {
  // there is a very important difference between chisel and c,
  // in chisel, mepc ## mcause ## mtvec ## mstatus, it indicates that
  // mstatus is on the least significant position, so we should use 
  // a software inversion to maintain visual consistency
  "mepc", "mcause", "mtvec", "mstatus",
};

uint64_t get_csrs_byidx(int idx, bool *success) {
    #include <macro.h>
    idx = ARRLEN(reg_csrs) - idx - 1;
    if (idx >= ARRLEN(reg_csrs)) {
        if (success) *success = false;
        return 0;
    }
    uint64_t val = dut.dbg_csrs[idx*2] + ((uint64_t)dut.dbg_csrs[idx*2+1]<<32);
    return val;
}


uint64_t get_csrs_byname(const char *s, bool *success) {
    #include <macro.h>

    #define suc(x) do {\
        if (success) *success = x;\
    } while(0)

    suc(false);
    
    int idx = -1;
    for (int i = 0; i < ARRLEN(reg_csrs); i++) {
        if (strcmp(s, reg_csrs[i]) == 0) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        G_DEBUG_W("%s is not in csr map", s);
        suc(false);
        return 0;
    }
    idx = ARRLEN(reg_csrs) - idx - 1;
    uint64_t val = dut.dbg_csrs[idx*2] + ((uint64_t)dut.dbg_csrs[idx*2+1]<<32);
    suc(true);
    return val;
    #undef suc
}

void rr_reg_display() 
{
  int i = 0;
  int temp;
  for (i = 0; i < 32; ++i) {
    G_DEBUG_WR("%3s\t=\t0x%08lx\n", reg_name(i), gpr(i));
  }
  for (int idx = 0; idx < ARRLEN(reg_csrs); idx++) {
    temp = ARRLEN(reg_csrs) - idx - 1;
    uint64_t val = dut.dbg_csrs[temp*2] + ((uint64_t)dut.dbg_csrs[temp*2+1]<<32);
    G_DEBUG_WR("%s\t=\t0x%016lx\n", reg_csrs[idx], val);
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
    bool suc;
    uint64_t val;
    val = get_csrs_byname(s, &suc);
    if (suc) {
      USE_SUC(true);
      return val;
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

  #define sync(csr) do {\
    cpu->csrs.csr = get_csrs_byname(#csr, NULL);\
    G_DEBUG_I("rr_wrap csr:%s, val: 0x%016lx", cpu->csrs.csr);\
  } while(0)
  sync(mepc);
  sync(mcause);
  sync(mtvec);
  sync(mstatus);
  #undef sync
}
