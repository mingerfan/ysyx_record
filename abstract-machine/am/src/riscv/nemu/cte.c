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

static void wraper(void *arg) {
  struct rt_func* func_struct = (struct rt_func*)arg;
  void (*entry)(void*) = func_struct->tentry;
  void (*exit)() = func_struct->texit;
  void (*free)(void*) = func_struct->free;
  entry(func_struct->param);
  free(func_struct);
  exit();
  // rt-thread保证代码不会从texit返回，因此返回是不正常的
  assert(0);
}

// 此后的修改将不支持yield-os
Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  (void)entry;
  struct rt_func* func_struct = (struct rt_func*)arg;
  Context *ctx = kstack.end;
  ctx = ctx - 1;
  ctx->mepc = (uintptr_t)wraper;
  ctx->gpr[10] = (uintptr_t)func_struct;
  // 这里需要给其赋予一个合法的值
  ctx->mstatus = 0xa00001800;
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
