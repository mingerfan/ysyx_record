#include <common.h>

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD: printf("Invoke yield fault\n"); break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  c->mepc = c->mepc + 4;

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
