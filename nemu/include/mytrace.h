#ifndef __MYTRACE_H__
#define __MYTRACE_H__

#include <cpu/cpu.h>
#include <cpu/decode.h>

#define IRINGBUF_SIZE 15
#define IRINGBUF_INFO_SIZE 128

extern char iringbuf[IRINGBUF_SIZE][IRINGBUF_INFO_SIZE];

void trace_inst_record(char *log);
void trace_inst_print();

void trace_memory_read(paddr_t addr, int len, word_t data);
void trace_memory_write(paddr_t addr, int len, word_t data);

void trace_ftrace_init(char *file);
void trace_ftrace_deinit();
void trace_ftrace_print(uint32_t inst, uint64_t pc);

#endif