#include <device.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

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
  const char *c = buf;
  for (int i = 0; i < len; i++) {
    putch(*(c+i));
  }
  return len;
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
  AM_GPU_CONFIG_T am_gpu_cfg;
  am_gpu_cfg = io_read(AM_GPU_CONFIG);
  int width = am_gpu_cfg.width;
  int height = am_gpu_cfg.height;
  return snprintf(buf, len, "WIDTH:%d\nHEIGHT:%d\n", width, height);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T am_gpu_cfg;
  am_gpu_cfg = io_read(AM_GPU_CONFIG);
  int width = am_gpu_cfg.width;
  int height = am_gpu_cfg.height;

  size_t pixoff = offset >> 2;
  size_t pixlen = len >> 2;
  assert(pixoff < width * height);

  int x = pixoff % width;
  int y = pixoff / width;
  assert(y * width + x < width * height);

  // AM_GPU_FBDRAW_T gpu_fb;
  // printf("GPU OFF: %ld, GPU_LEN: %ld, x: %d, y:%d\n", pixoff, pixlen, x, y);
  for (int i = 0; i < pixlen; i++) {
    // gpu_fb.x = x++;
    // gpu_fb.y = y;
    // gpu_fb.w = 1;
    // gpu_fb.h = 1;
    // gpu_fb.pixels = (*uint32_t)(uintptr_t)buf;
    // gpu_fb.sync = true;
    uint32_t *pixels = (uint32_t*)(uintptr_t)buf;
    io_write(AM_GPU_FBDRAW, x++, y, pixels + i, 1, 1, true);
    if (x >= width) {
      x = 0;
      y++;
    }
  }
  return len;
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
