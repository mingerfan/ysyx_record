#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <klib_tool.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  putstr("Not implemented\n");
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  prtDes pspec;
  int state = 0;
  const char *ftmp = fmt;

  int d;
  char *s;

  va_start(ap, fmt);
  while (*ftmp != '\0') {
    state = 0;
    switch (*ftmp) {
      case '%':
      ftmp = PrintDesDec(&pspec, ftmp);
      state = 1;
      break;

      default:
      *out++ = *ftmp++;
    }
    if (state == 1) {
      switch (pspec.Spec) {
        case PRINT_FAILED:
        assert(0);
        break;

        case PRINT_INT:
        d = va_arg(ap, int);
        out = Int2Char(out, d);
        break;

        case PRINT_STR:
        s = va_arg(ap, char*);
        out = StrCpyTool(out, s);
        break;
      }
    }
  }
  va_end(ap);
  *out = '\0';
  return StrCnt(out);
} 

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
