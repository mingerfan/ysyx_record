#include <common.h>
#include "syscall.h"
#define ENABLE_STRACE 1
#define STRACE() strace(c, __func__);
#define fn(x) static inline void x##_(Context *c)
#define call(x) x##_(c)

static inline void strace(Context *c, const char *name) {
#if ENABLE_STRACE
  printf("Current syscall is %s\n", name);
#endif
}

fn(SYS_yield) {
  STRACE();
  yield();
  c->GPRx = 0;
}

fn(SYS_exit) {
  STRACE();
  halt(0);
  // it seems no necessity to give return value
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_yield: call(SYS_yield); break;
    case SYS_exit : call(SYS_exit); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
