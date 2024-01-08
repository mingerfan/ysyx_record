#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <klib_tool.h>
#include <xprintf.h>

void putchar_(char c) {
  putch((uint8_t)c);
}

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// typedef void (*output_fn)(char c, void *data);

// int vformat(output_fn output, void *data, const char *format, ...) {

// }

int printf(const char *fmt, ...) {
  int cnt;
  va_list arp;
  va_start(arp, fmt);
  cnt = vprintf_(fmt, arp);
  va_end(arp);
  return cnt;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  int cnt;
  va_list arp;
  va_start(arp, fmt);
  cnt = vsnprintf_(out, -1, fmt, arp);
  va_end(arp);
  return cnt;
} 

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
