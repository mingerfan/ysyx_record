#include <device.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T am_key;
  am_key = io_read(AM_INPUT_KEYBRD);
  if (am_key.keycode == AM_KEY_NONE) {
    *(char*)buf = '\0';
    return 0;
  }
  return snprintf(buf, len, "%s %s\n", am_key.keydown ? "kd" : "ku", keyname[am_key.keycode]);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

int gettimeofday_(struct timeval *tv) {
  uint64_t us = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = us/1000000;
  tv->tv_usec = us%1000000;
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
