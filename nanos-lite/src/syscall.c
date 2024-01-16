#include <common.h>
#include <sys/time.h>
#include "syscall.h"
#include "fs.h"
#define ENABLE_STRACE 0
#define STRACE() strace(c, __func__);
#define fn(x) static inline void x##_(Context *c)
#define call(x) x##_(c)
#define ARG1 (c->GPR2)
#define ARG2 (c->GPR3)
#define ARG3 (c->GPR4)
#define RET(x) (c->GPRx = x) 

static inline void strace(Context *c, const char *name) {
#if ENABLE_STRACE
  printf("Current syscall is %s, args: %x, %x, %x\n", name, ARG1, ARG2, ARG3);
#endif
}

fn(SYS_yield) {
  STRACE();
  yield();
  RET(0);
}

fn(SYS_exit) {
  STRACE();
  halt(0);
  // it seems no necessity to give return value
}

fn(SYS_brk) {
  STRACE();
  RET(0);
}

fn(SYS_open) {
  STRACE();
  RET(fs_open((const char*)ARG1, ARG2, ARG3));
}

fn(SYS_read) {
  STRACE();
  RET(fs_read(ARG1, (void*)ARG2, ARG3));
}

fn(SYS_write) {
  STRACE();
  RET(fs_write(ARG1, (const void*)ARG2, ARG3));
}

fn(SYS_lseek) {
  STRACE();
  long unsigned int val;
  val = fs_lseek(ARG1, ARG2, ARG3);
  printf("offset val: %ld\n", val);
  RET(val);
}

fn(SYS_close) {
  STRACE();
  RET(fs_close(ARG1));
}

fn(SYS_gettimeofday) {
  int gettimeofday_(struct timeval *tv);
  assert((void*)ARG2 == NULL);
  RET(gettimeofday_((void*)ARG1));
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_yield: call(SYS_yield); break;
    case SYS_exit : call(SYS_exit); break;
    case SYS_brk  : call(SYS_brk); break;
    case SYS_open : call(SYS_open); break;
    case SYS_read : call(SYS_read); break;
    case SYS_write: call(SYS_write); break;
    case SYS_lseek: call(SYS_lseek); break;
    case SYS_close: call(SYS_close); break;
    case SYS_gettimeofday: call(SYS_gettimeofday); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
