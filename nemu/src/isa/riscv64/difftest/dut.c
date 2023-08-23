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
#include <cpu/difftest.h>
#include "../local-include/reg.h"

#define CHECK_CSR(x) do { if ((csr = *get_csr(#x)) != ref_r->csrs.x) {\
  printf("unmatched reg:%s DUT: %ld REF: %ld At 0x%lx\n", \
        #x, csr, ref_r->csrs.x, pc);\
  return false; \
} } while(0)

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  word_t csr;
  for (int i = 0; i < 32; i++) {
    if (gpr(i) != ref_r->gpr[i]) {
      printf("unmatched reg:%s DUT: %ld REF: %ld At 0x%lx\n", \
        reg_name(i, 0), gpr(i), ref_r->gpr[i], pc);
      return false;
    }
  }
  CHECK_CSR(mepc);
  CHECK_CSR(mcause);
  CHECK_CSR(mtvec);
  CHECK_CSR(mstatus);
  return true;
}

void isa_difftest_attach() {
}
