#include <am.h>
#include <riscv/npc_device.h>
#include <riscv/riscv.h>


void __am_uart_tx(AM_UART_TX_T *tx) {
    char c = tx->data;
    outb(SERIAL_PORT, c);
}