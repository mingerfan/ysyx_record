#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <klib_tool.h>
#include <xprintf.h>

void myput(int ch) {
  putch((uint8_t)ch);
}

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// typedef void (*output_fn)(char c, void *data);

// int vformat(output_fn output, void *data, const char *format, ...) {

// }

int printf(const char *fmt, ...) {
  va_list arp;
  va_start(arp, fmt);
  xvprintf(fmt, arp);
  va_end(arp);
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list arp;
  va_start(arp, fmt);
  xvsprintf(out, fmt, arp);
  va_end(arp);
  return 0;
} 

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
