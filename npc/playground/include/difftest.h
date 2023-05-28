#pragma once

#include <stdint.h>
#include <npc_memory.h>
#include <regs.h>
#include <generated/autoconf.h>

enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };
# define DIFFTEST_REG_SIZE (sizeof(uint64_t) * 33)

#ifdef CONFIG_DIFFTEST
void init_difftest(char *ref_so_file, long img_size, int port, CPU_state *cpu);
void difftest_skip_ref();
void difftest_step(vaddr_t pc, vaddr_t npc);
bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc);

#else

static inline void init_difftest(char *ref_so_file, long img_size, int port, CPU_state *cpu) {}
static inline void difftest_skip_ref() {}
static inline void difftest_step(vaddr_t pc, vaddr_t npc) {}
static inline bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) { return true; }

#endif