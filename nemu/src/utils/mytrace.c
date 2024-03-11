#include <mytrace.h>
#include <string.h>
#include <utils.h>
#include <generated/autoconf.h>
#include <readelf.h>
#include <macro.h>
#include "../isa/riscv64/local-include/reg.h"
#include <stdio.h>
#include <bindings.h>

char iringbuf[IRINGBUF_SIZE][IRINGBUF_INFO_SIZE] = {};
static int iringbuf_idx = 0;
static int first_idx = 0;

extern bool use_ftrace;
#ifdef CONFIG_FTRACE
// static int ftrace_deepth = 0;
// static int ftrace_printcnt = 0;
// static FILE *ftrace_fp = NULL;
static char *log_file = NULL;
#endif

#define immI() do { imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
#define immJ() do { \
  imm = (SEXT(BITS(i, 31, 31), 1) << 20) \
  | BITS(i, 19, 12) << 12 \
  | BITS(i, 20, 20) << 11 \
  | BITS(i, 30, 25) << 5 | BITS(i, 24, 21) << 1;} while(0)
#define immB() do { \
  imm = (SEXT(BITS(i, 31, 31), 1) << 12) \
  | BITS(i, 7, 7) << 11 \
  | BITS(i, 30, 25) << 5 | BITS(i, 11, 8) << 1; } while(0)


void trace_inst_record(char *log)
{
    strncpy(iringbuf[iringbuf_idx], log, IRINGBUF_INFO_SIZE-1);
    iringbuf[iringbuf_idx][IRINGBUF_INFO_SIZE-1] = '\0';
    iringbuf_idx = iringbuf_idx < IRINGBUF_SIZE-1 ? iringbuf_idx+1 : 0;
    if (first_idx < IRINGBUF_SIZE) first_idx++;
}

void trace_inst_print()
{
    log_write_en("\n------------------- iringbuf trace -------------------\n");
    int error_pos = iringbuf_idx - 1 < 0 ? IRINGBUF_SIZE - 1 : iringbuf_idx - 1;
    for (int i = 0; i < first_idx; ++i) {
        if (i == error_pos) {
            log_write_en("> ");
        }
        log_write_en("%s\n", iringbuf[i]);
    }
    log_write_en("------------------- iringbuf trace -------------------\n");
}

void trace_memory_read(paddr_t addr, int len, word_t data)
{
#ifdef CONFIG_MTRACE
    if (addr >= CONFIG_MTRACE_START && addr < CONFIG_MTRACE_END) {
        printf("MTrace--Read--at 0x%x--len: %d--data: 0x%016lx\n", addr, len, data);
    }
#endif
}

void trace_memory_write(paddr_t addr, int len, word_t data)
{
#ifdef CONFIG_MTRACE
    if (addr >= CONFIG_MTRACE_START && addr < CONFIG_MTRACE_END) {
        printf("MTrace--Write--at 0x%x--len: %d--data: 0x%016lx\n", addr, len, data);
    }
#endif
}

void trace_device_read(paddr_t addr, int len, IOMap *map, word_t data)
{
#ifdef CONFIG_DTRACE
    printf("DTrace--Read: %-8s--at 0x%016x--len: %d--data: 0x%016lx\n", map->name, addr, len, data);
#endif
}

void trace_device_write(paddr_t addr, int len, IOMap *map, word_t data)
{
#ifdef CONFIG_DTRACE
    printf("DTrace--Write: %-8s--at 0x%016x--len: %d--data: 0x%016lx\n", map->name, addr, len, data);
#endif
}

void trace_ftrace_start(char *file)
{
#ifdef CONFIG_FTRACE
    char *last_slash = strrchr(file, '/');
    int dir_len = last_slash - file + 1;
    log_file = malloc(dir_len + 30);
    strncpy(log_file, file, dir_len);
    strncpy(log_file + dir_len, "/ftrace_log.txt", 30);
    log_file[dir_len+30] = '\0';
    start_builder(file);
    set_show_context(true);
#endif
}

void trace_ftrace_add(char *file) {
#ifdef CONFIG_FTRACE
    add_prog_path(file);
#endif
}

void trace_ftrace_init() {
#ifdef CONFIG_FTRACE
    build_builder();
#endif
}

void trace_ftrace_deinit()
{
#ifdef CONFIG_FTRACE
    // ReaderDeInit();
    // fclose(ftrace_fp);
    // ftrace_fp = NULL;
#endif
}


#ifdef CONFIG_FTRACE
// static bool ftrace_is_jal(uint32_t inst)
// {
//     return BITS(inst, 6, 0) == 0b1101111;
// }

// static bool ftrace_is_jalr(uint32_t inst)
// {
//     return (BITS(inst, 6, 0) == 0b1100111) && (BITS(inst, 14, 12) == 0b000);
// }

// bool ftrace_jalr_ret(uint32_t) {
//     bool success;

// }

// static uint64_t jal_addr_calc(uint32_t inst, uint64_t pc) {
//     uint32_t i = inst;
//     uint64_t imm;
//     immJ();
//     return imm+pc;
// }

// static uint64_t jalr_addr_calc(uint32_t inst)
// {
//     uint32_t i = inst;
//     uint64_t imm;
//     immI();
//     return (imm+gpr(BITS(i, 19, 15))) & ~BITMASK(1);
// }

// static int ftrace_call_find(uint32_t inst, uint64_t pc) 
// {
//     uint64_t addr = 0;
//     if (ftrace_is_jal(inst)) {
//         addr = jal_addr_calc(inst, pc);
//     } else if (ftrace_is_jalr(inst)) {
//         addr = jalr_addr_calc(inst);
//     } else {
//         return -1;
//     }
//     for (int i = 0; i < elf_func_cnt; i++) {
//         if (addr == elf_func_syms[i].st_value) {
//             return i;
//         }
//     }
//     return -1;
// }

// static int ftrace_ret_find(uint32_t inst, uint32_t pc)
// {
//     if (ftrace_is_jalr(inst) && 
//     (BITS(inst, 19, 15) == 1) && (BITS(inst, 11, 7) == 0)) {
//         for (int i = 0; i < elf_func_cnt; i++) {
//             if (pc >= elf_func_syms[i].st_value && 
//                 pc < elf_func_syms[i].st_value + elf_func_syms[i].st_size) {
//                 return i;
//             }
//         }
//     }
//     return -1;
// }

// static void print_align(int gap)
// {
//     for (int i = 0; i < gap; i++) {
//         fprintf(ftrace_fp, "  ");
//     }
// }
#endif

void trace_ftrace_trace(uint32_t inst, uint64_t pc) {
#ifdef CONFIG_FTRACE
    extern CPU_state cpu;
    if (use_ftrace) {
        check_instruction(pc, inst, cpu.gpr);
    }
#endif
}

void trace_ftrace_print()
{
#ifdef CONFIG_FTRACE
    if (use_ftrace && log_file != NULL) {
        printf("Ftrace log at: %s\n", log_file);
        print_stack(log_file);
    }
#endif
}

void trace_etrace_ecall(CPU_state cpu, word_t NO) {
#ifdef CONFIG_ETRACE
    printf("ECALL! mepc: 0x%016lx, mtvec: 0x%016lx, NO: %ld, pc: 0x%08lx\n", cpu.csrs.mepc, cpu.csrs.mtvec, NO, cpu.pc);
#endif
}

void trace_etrace_mret() {
#ifdef CONFIG_ETRACE
    printf("Return from MRET!\n");
#endif
}