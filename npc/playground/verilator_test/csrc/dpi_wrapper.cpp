#include <Vtop__Dpi.h>
#include "state.h"
#include "debug.h"
#include "npc_memory.h"
#include "macro.h"
#include "device.h"
#include "utils.h"
#include "difftest.h"

void invalid_inst_(unsigned long long c_pc)
{
    Assert(sizeof(unsigned long long)==sizeof(uint64_t), "type unmatch!");
    if (c_pc >= CONFIG_NPC_PC && npc_state.state == NPC_RUNNING) {
        invalid_inst(c_pc);
    }
}

void set_state_end_(unsigned long long c_pc, int halt_ret)
{
    Assert(sizeof(unsigned long long)==sizeof(uint64_t), "type unmatch!");
    set_state_end(c_pc, halt_ret);
}

void pmem_read(long long raddr, long long *rdata) {
    if (npc_state.state == NPC_RUNNING) {
        // 总是读取地址为`raddr & ~0x7ull`的8字节返回给`rdata`
        if (raddr < CONFIG_NPC_PC) { 
            npc_state.state == NPC_ABORT; 
            G_DEBUG_E("pmem addr read err at 0x%016llx", raddr);
            return; 
        }
        if (raddr == RTC_ADDR) {
            difftest_skip_ref();
            *rdata = get_time();
        }
        else {
            *rdata = paddr_read(raddr & ~0x7ull, 8);
        }
        G_DEBUG_I("raddr: 0x%016lx rdata: 0x%016llx", raddr, *rdata);
    } else {
        *rdata = 0;
    }
}

void inst_read(long long pc, long long *rdata) {
    if (pc >= CONFIG_NPC_PC) {
        *rdata = paddr_read(pc, 4);
    }
}

void pmem_write(long long waddr, long long wdata, char wmask) {
    if (npc_state.state == NPC_RUNNING) {
        // 总是往地址为`waddr & ~0x7ull`的8字节按写掩码`wmask`写入`wdata`
        // `wmask`中每比特表示`wdata`中1个字节的掩码,
        // 如`wmask = 0x3`代表只写入最低2个字节, 内存中的其它字节保持不变
        if (waddr < CONFIG_NPC_PC) { 
            npc_state.state == NPC_ABORT; 
            G_DEBUG_E("pmem addr write err at 0x%016llx", waddr);
            return; 
        }
        G_DEBUG_I("waddr: 0x%016lx wmask:0x%016lx", waddr, wmask);

        if (waddr == SERIAL_PORT) {
            difftest_skip_ref();
            putchar((uint8_t)wdata);
            return;
        } 

        uint64_t val = paddr_read(waddr & ~0x7ull, 8);
        uint64_t res = 0;
        for (int i = 0; i < 8; i++) {
            if (BITS((uint64_t)wmask, i, i) == 1) {
                res |= BITS(wdata, (i+1)*8-1, i*8) << (i*8);
            } else {
                res |= BITS(val, (i+1)*8-1, i*8) << (i*8);
            }
        }
        paddr_write(waddr & ~0x7ull, 8, res);

        G_DEBUG_I("wdata: 0x%016lx waddr:0x%016lx val: 0x%016lx res: 0x%016lx",
        wdata, waddr & ~0x7ull, val, res);
    }
}