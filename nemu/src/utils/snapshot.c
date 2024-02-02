#include <stdio.h>
// #include <src/isa/riscv64/include/isa-def.h>
#include <isa.h>
#include <memory/paddr.h>

void save_snapshot(const char *path) {
    FILE *file;

    file = fopen(path, "wb");

    if (file == NULL) {
        printf("Warning: Can not open file!");
    }
    printf("pc: 0x%lx, x2: 0x%lx\n", cpu.pc, cpu.gpr[2]);
    assert(fwrite(&cpu, sizeof(CPU_state), 1, file) == 1);
    assert(fwrite(guest_to_host(PMEM_LEFT), PMEM_RIGHT - PMEM_LEFT + 1, 1, file) == 1);
    fclose(file);
}

void load_snapshot(const char *path) {
    FILE *file;
    file = fopen(path, "rb");
    
    if (file == NULL) {
        printf("Warning: Can not open file");
    }

    assert(fread(&cpu, sizeof(CPU_state), 1, file) == 1);
    printf("pc: 0x%lx, x2: 0x%lx\n", cpu.pc, cpu.gpr[2]);
    assert(fread(guest_to_host(PMEM_LEFT), PMEM_RIGHT - PMEM_LEFT + 1, 1, file) == 1);
    fclose(file);
}