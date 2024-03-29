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
#include <macro.h>
#include <mytrace.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.csrs.mepc = epc;
  cpu.csrs.mcause = 0xb;
  trace_etrace_ecall(cpu, NO);
  // reset MPIE
  cpu.csrs.mstatus &= ~(word_t)(1 << 7);
  // set MPP = 11b, MPIE = MIE
  cpu.csrs.mstatus |= (1 << 11 | 1 << 12 | BITS(cpu.csrs.mstatus, 3, 3) << 7);
  return cpu.csrs.mtvec & ~0x3;
}

word_t isa_return_intr() {
  // sets MPV(not in mstatus)=0, MPP[12:11]=0, MIE[3]=MPIE[7], and MPIE=1
  cpu.csrs.mstatus &= ~(word_t)(1 << 12 | 1 << 11 | 1 << 3);
  cpu.csrs.mstatus |= (BITS(cpu.csrs.mstatus, 7, 7) << 3 | 1 << 7);
  trace_etrace_mret();
  return cpu.csrs.mepc;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
