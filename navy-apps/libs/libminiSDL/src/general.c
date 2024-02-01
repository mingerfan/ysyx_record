#include <NDL.h>
#include <assert.h>
#include <stdint.h>

int SDL_Init(uint32_t flags) {
  NDL_Init(flags);
  extern uint32_t sdl_boot_time;
  sdl_boot_time = NDL_GetTicks();
  return 0;
}

void SDL_Quit() {
  NDL_Quit();
}

char *SDL_GetError() {
  return "Navy does not support SDL_GetError()";
}

int SDL_SetError(const char* fmt, ...) {
  assert(0);
  return -1;
}

int SDL_ShowCursor(int toggle) {
  assert(0);
  return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
}
