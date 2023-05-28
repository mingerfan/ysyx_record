#ifndef __REGS_H__
#define __REGS_H__

#include <stdint.h>
#include <debug.h>
#include <Vtop.h>
#include <npc_memory.h>

extern TOP_NAME dut;

struct CPU_state {
    word_t gpr[32];
    vaddr_t pc;
};

static inline uint64_t get_regs(int num)
{
    assert(num >= 0 && num < 32);
    uint64_t val =  dut.dbg_regs[num*2] + ((uint64_t)dut.dbg_regs[num*2+1]<<32);
    return val;
}

static inline uint64_t get_pc() {
    uint64_t val = dut.io_pc;
    return val;
}

#define gpr(idx) (get_regs(idx))

static inline const char* reg_name(int idx) 
{
    extern const char* regs[];
    assert(idx >= 0 && idx < 32);
    return regs[idx];
}

void rr_reg_display();
uint64_t rr_reg_str2val(const char *s, bool *success);
void rr_wrap(CPU_state *cpu);

#endif