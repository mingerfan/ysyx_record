#include <am.h>
#include <nemu.h>
#include <riscv/riscv.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t kbd_raw = inl(KBD_ADDR);
  if ((kbd_raw & KEYDOWN_MASK) == KEYDOWN_MASK) {
    kbd->keydown = true;
    kbd->keycode = kbd_raw & ~KEYDOWN_MASK;
  } else {
    kbd->keydown = false;
    kbd->keycode = AM_KEY_NONE;
  }
}
