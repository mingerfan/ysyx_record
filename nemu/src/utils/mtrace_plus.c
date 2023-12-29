#include <utils.h>
#include <macro.h>
#include "../isa/riscv64/local-include/reg.h"
#include <stdio.h>
#include <isa.h>
#include <generated/autoconf.h>
#include "mtrace_plus.h"

typedef struct meta_data {
    int idx;
    uint32_t inst;
} meta_data;

static int idx = MT_FLAG_END;
static bool print = false;

static meta_data MT_LIST[MT_FLAG_END] = {0};
static char MT_NAME[MT_FLAG_END][5] = {
    [MT_LD_FLAG] = "ld",
    [MT_LW_FLAG] = "lw",
    [MT_LWU_FLAG] = "lwu",
    [MT_LH_FLAG] = "lh",
    [MT_LHU_FLAG] = "lhu",
    [MT_LB_FLAG] = "lb",
    [MT_LBU_FLAG] = "lbu",
    [MT_SD_FLAG] = "sd",
    [MT_SW_FLAG] = "sw",
    [MT_SH_FLAG] = "sh",
    [MT_SB_FLAG] = "sb",
};

void trigger_state(int idx_, uint32_t inst) {
#ifdef CONFIG_MTRACE
    idx = idx_;
    MT_LIST[idx].inst = inst;
#endif
}

static bool hit_MT() {
#ifdef CONFIG_MTRACE
    if (enable_mtrace_plus() && idx >= 0 && idx < MT_FLAG_END) {
        MT_LIST[idx].idx = 1;
        return true;
    }
#endif
    return false;
}   


bool enable_mtrace_plus() 
{
    return true;
}

void get_inst_print(char *s)
{
#ifdef CONFIG_IRINGBUF
    // current or last instruction
    if (print) {
        printf(ANSI_FG_CYAN"mtrace inst: %s\n\n"ANSI_NONE, s);
        print = false;
    }
#elif
    return NULL;
#endif
}

static bool filter(paddr_t addr, int len, word_t data) {
    int rd  = BITS(MT_LIST[idx].inst, 11, 7);
    // int rs1 = BITS(MT_LIST[idx].inst, 19, 15);
    // int rs2 = BITS(MT_LIST[idx].inst, 24, 20);
    if (rd == 10) {
        return true;
    }
    return false;
}

void mtrace_plus(paddr_t addr, int len, word_t data)
{
#ifdef CONFIG_MTRACE
    if (hit_MT() && filter(addr, len, data)) {
        printf(ANSI_FG_MAGENTA"inst: %s , addr: 0x%016x, len: %d, data: 0x%016lx\n"ANSI_NONE, MT_NAME[idx], addr, len, data);
        idx = MT_FLAG_END;
        print = true;
    }
    for (int i = 0; i < MT_FLAG_END; i++) {
        MT_LIST[i].idx = 0;
    }
#endif
}