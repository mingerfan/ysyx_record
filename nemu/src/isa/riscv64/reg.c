/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include "local-include/reg.h"
#include "debug.h"

static int csr_index = 0;

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

const char *csrs[] = {
  "mepc", "mcause", "mtvec", "mstatus"
};

word_t* get_csr(const char *s) {
  #define match(x) (strcmp(x, s) == 0)
  if (match("mepc")) {
    return &cpu.csrs.mepc;
  } else if (match("mcause")) {
    return &cpu.csrs.mcause;
  } else if (match("mtvec")) {
    return &cpu.csrs.mtvec;
  } else if (match("mstatus")) {
    return &cpu.csrs.mstatus;
  }
  #undef match
  return NULL;
}

word_t* get_csr_bynum(uint64_t csr_addr) {
  switch (csr_addr)
  {
    case CSR_MEPC:          return &cpu.csrs.mepc;
    case CSR_MCAUSE:        return &cpu.csrs.mcause;
    case CSR_MTVEC:         return &cpu.csrs.mtvec;
    case CSR_MSTATUS:       return &cpu.csrs.mstatus;
    default: panic("This csr is not implemented! addr: 0x%lx", csr_addr);
  }
  return NULL;
}

word_t* csr_start() {
  csr_index = 0;
  return get_csr(csrs[0]);
}

word_t* csr_next() {
  return get_csr(csrs[csr_index++]);
}

void isa_reg_display() {
  int i = 0;
  word_t *csr;
  for (i = 0; i < 32; ++i) {
    printf("%3s\t=\t0x%08lx\n", reg_name(i, 0), gpr(i));
  }
  for (i = 0; i < ARRLEN(csrs); ++i) {
    assert((csr = get_csr(csrs[i])));
    printf("%3s\t=\t0x%08lx\n", csrs[i], *csr);
  }
}

word_t isa_reg_str2val(const char *s, bool *success) {
  word_t *csr;
  *success = false;
  if (strcmp("pc", s) == 0) {
    *success = true;
    return cpu.pc;
  }
  if ((csr = get_csr(s)) != NULL) {
    *success = true;
    return *csr;
  }
  for (int i = 0; i < ARRLEN(regs); ++i) {
    if (strcmp(regs[i], s) == 0) {
      *success = true;
      return cpu.gpr[i];
    }
  }
  return 0;
}
