#ifndef __MYTRACE_H__
#define __MYTRACE_H__

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <device/map.h>

#define IRINGBUF_SIZE 15
#define IRINGBUF_INFO_SIZE 128

extern char iringbuf[IRINGBUF_SIZE][IRINGBUF_INFO_SIZE];

void trace_inst_record(char *log);
void trace_inst_print();

void trace_memory_read(paddr_t addr, int len, word_t data);
void trace_memory_write(paddr_t addr, int len, word_t data);

void trace_ftrace_start(char *file);
void trace_ftrace_add(char *file);
void trace_ftrace_init();
void trace_ftrace_deinit();
void trace_ftrace_trace(uint32_t inst, uint64_t pc);
void trace_ftrace_print();

void trace_device_read(paddr_t addr, int len, IOMap *map, word_t data);
void trace_device_write(paddr_t addr, int len, IOMap *map, word_t data);

void trace_etrace_ecall(CPU_state cpu, word_t NO);
void trace_etrace_mret();

#endif