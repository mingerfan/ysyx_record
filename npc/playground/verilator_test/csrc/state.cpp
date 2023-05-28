#include "state.h"
#include "debug.h"

NPCState npc_state = { .state = NPC_STOP };

void set_npc_state(int state, uint64_t pc, int halt_ret)
{
    npc_state.state = state;
    npc_state.halt_pc = pc;
    npc_state.halt_ret = halt_ret;
}

void set_state_end(uint64_t pc, int halt_ret)
{
    npc_state.state = NPC_END;
    npc_state.halt_pc = pc;
    npc_state.halt_ret = halt_ret;
}

void invalid_inst(uint64_t pc)
{
    npc_state.state = NPC_ABORT;
    npc_state.halt_pc = pc;
    npc_state.halt_ret = -1;
    G_DEBUG_E("INVALID INST!");
}

void state_check()
{
    switch (npc_state.state)
    {
        case NPC_RUNNING: npc_state.state = NPC_STOP; break;
        case NPC_END: case NPC_ABORT:
        {
            if (npc_state.state == NPC_ABORT) G_DEBUG_WR(ANSI_FG_RED "ABORT\n" ANSI_NONE);
            else if (npc_state.state == NPC_END) {
                if (npc_state.halt_ret == 0) G_DEBUG_WR(ANSI_FG_GREEN "HIT GOOD TRAP\n" ANSI_NONE);
                else G_DEBUG_WR(ANSI_FG_RED "HIT BAD TRAP\n" ANSI_NONE);
            }
        }
    }
}