#ifndef __STATE_H__
#define __STATE_H__

#include <stdint.h>

enum { NPC_RUNNING, NPC_STOP, NPC_END, NPC_ABORT, NPC_QUIT };

typedef struct
{
    int state;
    uint64_t halt_pc;
    uint32_t halt_ret;
} NPCState;

extern NPCState npc_state;

void set_npc_state(int state, uint64_t pc, int halt_ret);
void set_state_end(uint64_t pc, int halt_ret);
void invalid_inst(uint64_t pc);
void state_check();
int state_check_return();

#endif