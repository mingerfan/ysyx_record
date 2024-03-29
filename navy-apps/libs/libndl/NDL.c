#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  assert(gettimeofday(&tv, NULL) == 0);
  return tv.tv_sec*1000 + tv.tv_usec/1000;
}

int NDL_PollEvent(char *buf, int len) {
  int evtdev = open("/dev/events", O_RDONLY);
  return read(evtdev, buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    assert(write(fbctl, buf, len) != -1);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  if (*w > screen_w) *w = screen_w;
  if (*h > screen_h) *h = screen_h;
  if (*w == 0 && *h == 0) {
    *w = screen_w;
    *h = screen_h;
  }
  canvas_w = *w;
  canvas_h = *h;
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  assert( x + w <= canvas_w );
  assert( y + h <= canvas_h );
  int canvasx_0 = (screen_w - canvas_w) / 2;
  int canvasy_0 = (screen_h - canvas_h) / 2;
  x = canvasx_0 + x;
  y = canvasy_0 + y;
  int fb = open("/dev/fb", O_WRONLY);
  for (int i = y; i < y + h; i++) {
    // write(fb, pixels, w);
    lseek(fb, (i * screen_w + x) * 4, SEEK_SET);
    assert(write(fb, pixels, w * 4) != -1);
    pixels += w;
  }
  close(fb);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  int dispinfo = open("/proc/dispinfo", O_RDONLY);
  char buf[30];
  assert(read(dispinfo, buf, 30) != -1);
  char *newline = strchr(buf, '\n');
  assert(newline);
  assert(sscanf(buf, "WIDTH:%d", &screen_w) == 1);
  assert(sscanf(newline + 1, "HEIGHT:%d", &screen_h) == 1);
  printf("SCREEN WIDTH: %d, SCREEN HEIGHT: %d\n", screen_w, screen_h);
  close(dispinfo);

  return 0;
}

void NDL_Quit() {
}
