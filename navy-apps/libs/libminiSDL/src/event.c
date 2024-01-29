#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

static int key_cmp(char *key) {
  // printf("arr len: %ld\n", sizeof(keyname)/sizeof(keyname[0]));
  for (int i = 0; i < sizeof(keyname)/sizeof(keyname[0]); i++) {
    if (strcmp(key, keyname[i]) == 0) return i;
  }
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char event_s[20], k_status[3], key[15];
  if (NDL_PollEvent(event_s, 19) > 0) {
    assert(sscanf(event_s, "%s %s", k_status, key) == 2);
    // printf("k_status: %s, key: %s\n", k_status, key);
    if (strcmp(k_status, "kd") == 0) {
      ev->key.type = SDL_KEYDOWN;
      ev->key.keysym.sym = key_cmp(key);
    } else if (strcmp(k_status, "ku") == 0) {
      ev->key.type = SDL_KEYUP;
      ev->key.keysym.sym = key_cmp(key);
    }
    return 1;
  }
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  char event_s[20], k_status[3], key[15];
  while (1) {
    int res = NDL_PollEvent(event_s, 19);
    if (res > 0) {
      assert(sscanf(event_s, "%s %s", k_status, key) == 2);
      // printf("k_status: %s, key: %s\n", k_status, key);
      if (strcmp(k_status, "kd") == 0) {
        event->key.type = SDL_KEYDOWN;
        event->key.keysym.sym = key_cmp(key);
      } else if (strcmp(k_status, "ku") == 0) {
        event->key.type = SDL_KEYUP;
        event->key.keysym.sym = key_cmp(key);
      }
      return 1;
    } else if (res < 0) {
      return 0;
    }
  }
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  assert(0);
  return NULL;
}
