#include <Vtop__Dpi.h>
#include "state.h"
#include "debug.h"
#include "npc_memory.h"
#include "macro.h"

void invalid_inst_(unsigned long long c_pc)
{
    Assert(sizeof(unsigned long long)==sizeof(uint64_t), "type unmatch!");
    if (c_pc >= CONFIG_NPC_PC) {
        invalid_inst(c_pc);
    }
}

void set_state_end_(unsigned long long c_pc, int halt_ret)
{
    Assert(sizeof(unsigned long long)==sizeof(uint64_t), "type unmatch!");
    set_state_end(c_pc, halt_ret);
}

void pmem_read(long long raddr, long long *rdata) {
    // 总是读取地址为`raddr & ~0x7ull`的8字节返回给`rdata`
    paddr_read(raddr & ~0x7ull, 8);
}

void pmem_write(long long waddr, long long wdata, char wmask) {
    // 总是往地址为`waddr & ~0x7ull`的8字节按写掩码`wmask`写入`wdata`
    // `wmask`中每比特表示`wdata`中1个字节的掩码,
    // 如`wmask = 0x3`代表只写入最低2个字节, 内存中的其它字节保持不变
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
}