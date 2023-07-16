#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
#define S_W 400
#define S_H 300

void __am_gpu_init() {
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t vga_info = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = vga_info >> 16, .height = (vga_info & 0x0000ffff),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (int y = 0; y < ctl->h; y++) {
    for (int x = 0; x < ctl->w; x++) {
      uintptr_t addr = S_W*(ctl->y+y) + (ctl->x + x);
      uint32_t *pixel = ctl->pixels;
      fb[addr] = *(pixel+ y*ctl->w + x);
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
