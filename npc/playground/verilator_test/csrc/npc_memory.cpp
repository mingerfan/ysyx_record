#include "npc_memory.h"
#include "string.h"
#include <stdio.h>
#include "debug.h"

long npcmem_img_size = 0;

uint8_t pmem[CONFIG_NPC_MSIZE] PG_ALIGN = {};
static const uint32_t init_arr[] = {
    // 0x00100093, // addi x1, x0, 0x1
    // 0x00108093, // addi x1, x1, 0x1
    // 0x00100073, // ebreak
    0x00000413,
    0x00009117,
    0xffc10113,
    0x00c000ef,
    0x00000513,
    0x00100073,
    0xff010113,
    0x00000517,
    0x01c50513,
    0x00113423,
    0xfe9ff0ef,
};

uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_NPC_PC; }
paddr_t host_to_guset(uint8_t *addr) { return addr - pmem + CONFIG_NPC_PC; }

static uint64_t pmem_read_base(paddr_t paddr, int len)
{
    Assert(0 >= CONFIG_NPC_PC, "fas");
    Assert(paddr >= CONFIG_NPC_PC, "Addr is less than CONFIG_NPC_PC");
    return host_read(guest_to_host(paddr), len);
}

static void pmem_write_base(paddr_t paddr, int len, uint64_t data)
{
    Assert(paddr >= CONFIG_NPC_PC, "Addr is less than CONFIG_NPC_PC");
    host_write(guest_to_host(paddr), len, data);
}

// We don't care address check and device read now
uint64_t paddr_read(paddr_t paddr, int len)
{
    return pmem_read_base(paddr, len);
}

void paddr_write(paddr_t paddr, int len, uint64_t data)
{
    pmem_write_base(paddr, len, data);
}

uint64_t vaddr_read(vaddr_t addr, int len) 
{
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint64_t data) 
{
  paddr_write(addr, len, data);
}

static long load_img(const char *img_file)
{
    if (img_file == NULL) {
        G_DEBUG_I("No image file. Use build-in image\n");
        return 4096;
    }
    FILE *fp = fopen(img_file, "rb");

    Assert(fp, "Can not open '%s'", img_file);

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);

    G_DEBUG_WR("The image is %s, size = %ld\n", img_file, size);

    fseek(fp, 0, SEEK_SET);
    int ret = fread(guest_to_host(CONFIG_NPC_PC), size, 1, fp);
    assert(ret == 1);

    fclose(fp);
    return size;
}

void mem_init(const char *bin_path)
{
    npcmem_img_size = 0;
    if (bin_path == NULL) {
        G_DEBUG_I("No image file. Use build-in image");
        memcpy(guest_to_host(CONFIG_NPC_PC), init_arr, sizeof(init_arr));
    } else {
        npcmem_img_size = load_img(bin_path);
    }
}
