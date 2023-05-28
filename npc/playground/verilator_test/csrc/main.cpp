#include <stdio.h>
#include <nvboard.h>
#include <Vtop.h>
#include <generated/autoconf.h>

static TOP_NAME dut;

#ifdef CONFIG_NVBOARD_ON
void nvboard_bind_all_pins(TOP_NAME* top);
#endif

static void single_cycle() {
  dut.clock = 0; dut.eval();
  dut.clock = 1; dut.eval();
}

static void reset(int n) {
  dut.reset = 1;
  while (n -- > 0) single_cycle();
  dut.reset = 0;
}

#ifndef CONFIG_NPC
int main() {
  #ifdef CONFIG_NVBOARD_ON
  nvboard_bind_all_pins(&dut);
  nvboard_init();
  #endif

  reset(10);

  while(1) {
    #ifdef CONFIG_NVBOARD_ON
    nvboard_update();
    #endif
    single_cycle();
  }
}
#endif
