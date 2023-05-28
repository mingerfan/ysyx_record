#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void init_disasm(const char *triple);
void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);

#ifdef __cplusplus
}
#endif