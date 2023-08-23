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

#ifndef __RISCV64_REG_H__
#define __RISCV64_REG_H__

#include <common.h>

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < 32));
  return idx;
}

#define gpr(idx) (cpu.gpr[check_reg_idx(idx)])
#define csr(addr) (get_csr_bynum(addr))

static inline const char* reg_name(int idx, int width) {
  extern const char* regs[];
  return regs[check_reg_idx(idx)];
}

void isa_reg_display();
word_t isa_reg_str2val(const char *s, bool *success);
word_t* get_csr(const char *s);
word_t* get_csr_bynum(uint64_t csr_addr);
word_t* csr_start();
word_t* csr_next();

#define CSRS_ITER(csr) for (word_t *csr = csr_start(); csr != NULL; csr = csr_next())

#endif
