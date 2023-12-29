#pragma once

#include "common.h"
#include <mytrace.h>

enum MT_INST_FLAG {
    MT_LD_FLAG = 0,
    MT_LW_FLAG,
    MT_LWU_FLAG,
    MT_LH_FLAG,
    MT_LHU_FLAG,
    MT_LB_FLAG,
    MT_LBU_FLAG,
    MT_SD_FLAG,
    MT_SW_FLAG,
    MT_SH_FLAG,
    MT_SB_FLAG,
    MT_FLAG_END
};

void trigger_state(int idx_, uint32_t inst);
bool enable_mtrace_plus();
void get_inst_print(char *s);
void mtrace_plus(paddr_t addr, int len, word_t data);
