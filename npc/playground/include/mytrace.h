#ifndef __MYTRACE_H__
#define __MYTRACE_H__

#include <stdint.h>
#include <generated/autoconf.h>

#define IRINGBUF_SIZE 128
#define IRINGBUF_INFO_SIZE 128

extern char iringbuf[IRINGBUF_SIZE][IRINGBUF_INFO_SIZE];

#ifdef CONFIG_ITRACE
void trace_inst_record(char *log);
void trace_inst_print(int mode);
void itrace_init(char *log_path);
void itrace_recorde();
void itrace_deinit();
#else 

static inline void trace_inst_record(char *log) {}
static inline void trace_inst_print(int mode) {}
static inline void itrace_init(char *log_path) {}
static inline void itrace_recorde() {}
static inline void itrace_deinit() {}
#endif

void trace_ftrace_start(char *file);
void trace_ftrace_add(char *file);
void trace_ftrace_init();
void trace_ftrace_deinit();
void trace_ftrace_trace(uint32_t inst, uint64_t pc);
void trace_ftrace_print();

#endif