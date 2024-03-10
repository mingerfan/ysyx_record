#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
#include "../../../include/arch/riscv64-nemu.h"

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  // printf("mcause: 0x%lx, mstatus: 0x%lx, mepc: 0x%lx\n", c->mcause, c->mstatus, c->mepc);
  if (user_handler) {
    Event ev = {0};
    if (c->GPR1 == -1) {
      ev.event = EVENT_YIELD;
    } else if (c->GPR1 >= 0 && c->GPR1 <= 19) {
      ev.event = EVENT_SYSCALL;
    } else {
      ev.event = EVENT_ERROR;
    }
    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *ctx = kstack.end;
  ctx = ctx - 1;
  ctx->mepc = (uintptr_t)entry - 4;
  ctx->gpr[10] = (uintptr_t)arg;
  return ctx;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
