#include <am.h>
#include <klib-macros.h>

void __am_timer_init();

void __am_timer_rtc(AM_TIMER_RTC_T *);
void __am_timer_uptime(AM_TIMER_UPTIME_T *);
void __am_input_keybrd(AM_INPUT_KEYBRD_T *);
void __am_timer_rtc(AM_TIMER_RTC_T *);
void __am_uart_tx(AM_UART_TX_T *);

static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) { cfg->present = true; cfg->has_rtc = true; }
static void __am_input_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = true;  }

static inline void _dummy(AM_AUDIO_CONFIG_T *cfg) {}

typedef void (*handler_t)(void *buf);
static void *lut[128] = {
  [AM_TIMER_CONFIG] = __am_timer_config,
  [AM_TIMER_RTC   ] = __am_timer_rtc,
  [AM_TIMER_UPTIME] = __am_timer_uptime,
  [AM_INPUT_CONFIG] = __am_input_config,
  [AM_INPUT_KEYBRD] = __am_input_keybrd,
  [AM_UART_TX]      = __am_uart_tx,
  [AM_GPU_CONFIG  ] = _dummy,
  [AM_GPU_FBDRAW  ] = _dummy,
  [AM_GPU_STATUS  ] = _dummy,
  [AM_UART_CONFIG ] = _dummy,
  [AM_AUDIO_CONFIG] = _dummy,
  [AM_AUDIO_CTRL  ] = _dummy,
  [AM_AUDIO_STATUS] = _dummy,
  [AM_AUDIO_PLAY  ] = _dummy,
  [AM_DISK_CONFIG ] = _dummy,
  [AM_DISK_STATUS ] = _dummy,
  [AM_DISK_BLKIO  ] = _dummy,
  [AM_NET_CONFIG  ] = _dummy,
};

static void fail(void *buf) { panic("access nonexist register"); }

bool ioe_init() {
  for (int i = 0; i < LENGTH(lut); i++)
    if (!lut[i]) lut[i] = fail;
  __am_timer_init();
  return true;
}

void ioe_read (int reg, void *buf) { ((handler_t)lut[reg])(buf); }
void ioe_write(int reg, void *buf) { ((handler_t)lut[reg])(buf); }
