#ifndef __NPC_MEMORY_H
#define __NPC_MEMORY_H

#include "generated/autoconf.h"
#include "stdint.h"

#define PG_ALIGN __attribute((aligned(4096)))

static inline uint64_t host_read(void *addr, int len)
{
    switch (len) {
        case 1: return *(uint8_t    *)addr;
        case 2: return *(uint16_t   *)addr;
        case 4: return *(uint32_t   *)addr;
        case 8: return *(uint64_t   *)addr;
        default:
        return 0;
    }
}

static inline void host_write(void *addr, int len, uint64_t data)
{
    switch (len) {
        case 1: *(uint8_t   *)addr = (uint8_t   )data; return;
        case 2: *(uint16_t  *)addr = (uint16_t  )data; return;
        case 4: *(uint32_t  *)addr = (uint32_t  )data; return;
        case 8: *(uint64_t  *)addr = (uint64_t  )data; return;
        default:
        return;
    }
}

typedef uint64_t paddr_t;
typedef uint64_t vaddr_t;
typedef uint64_t word_t;

uint64_t vaddr_read(vaddr_t addr, int len);
void vaddr_write(vaddr_t addr, int len, uint64_t data); 

uint8_t* guest_to_host(paddr_t paddr);
paddr_t host_to_guset(uint8_t *addr);
uint64_t paddr_read(paddr_t paddr, int len);
void paddr_write(paddr_t paddr, int len, uint64_t data);
void mem_init(const char *bin_path);

#endif