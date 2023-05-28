#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  assert(s != NULL);
  int cnt = 0;
  while (*s != '\0') {
    ++cnt;
    ++s;
  }
  return cnt;
}

char *strcpy(char *dst, const char *src) {
  assert((dst!=NULL)&&(src!=NULL));
  char *temp = dst;
  while ((*dst++ = *src++) != '\0');
  return temp;
}

char *strncpy(char *dst, const char *src, size_t n) {
  assert((dst!=NULL)&&(src!=NULL));
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; ++i) {
    dst[i] = src[i];
  }
  for (; i < n; ++i) {
    dst[i] = '\0';
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  assert((dst!=NULL)&&(src!=NULL));
  size_t i = 0;
  while (dst[i]!='\0') {
    ++i;
  }
  for (size_t j = 0; src[j] != '\0'; ++j) {
    dst[i++] = src[j]; 
  }
  dst[i] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  assert((s1!=NULL)&&(s2!=NULL));
  int ret = 0;
  while (!(ret = ((unsigned char)*s1 - (unsigned char)*s2)) && *s1 != '\0') {
    s1++;
    s2++;
  }
  return ret;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  assert((s1!=NULL)&&(s2!=NULL));
  int ret = 0;
  for (int i = 0; i < n; ++i) {
    if (!(ret = ((unsigned char)*s1 - (unsigned char)*s2)) && *s1 != '\0') {
      s1++;
      s2++;
    } else {
      break;
    }
  }
  return ret;
}

void *memset(void *s, int c, size_t n) {
  assert(s!=NULL);
  char *s_ = s;
  for (int i = 0; i < n; ++i) {
    *(s_+i) = (char)c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  assert((dst!=NULL)&&(src!=NULL));
  if (n == 0) {
    return NULL;
  }
  char *dst_ = dst;
  const char *src_ = src;
  if (dst_ <= src_ || dst_ >= src_ + n) {
    while (n--) {
      *dst_++ = *src_++;
    }
  } else {
    dst_ = dst_ + n - 1;
    src_ = src_ + n - 1;
    while (n--) {
      *dst_-- = *src_--;
    }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  assert((out!=NULL)&&(in!=NULL));
  if (n == 0) {
    return NULL;
  }
  char *out_ = out;
  const char *in_ = in;
  while (n--) {
    *out_++ = *in_++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  assert((s1!=NULL)&&(s2!=NULL));
  int ret = 0;
  const unsigned char *s1_ = s1;
  const unsigned char *s2_ = s2;
  for (int i = 0; i < n; ++i) {
    if (!(ret = (*s1_ - *s2_))) {
      s1_++;
      s2_++;
    } else {
      break;
    }
  }
  return ret;
}

#endif
