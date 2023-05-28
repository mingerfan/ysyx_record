#include <mytrace.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <npc_memory.h>
#include <Vtop.h>
#include <disasm.h>
#include <debug.h>
#include <stdio.h>
#include <macro.h>
#include <readelf.h>
#include <regs.h>

static char buf[128];
extern TOP_NAME dut;
static FILE *f = nullptr;
static int count = 0;

char iringbuf[IRINGBUF_SIZE][IRINGBUF_INFO_SIZE] = {};
static int iringbuf_idx = 0;
static int first_idx = 0;

static int ftrace_deepth = 0;
static int ftrace_printcnt = 0;
static FILE *ftrace_fp = nullptr;

#ifdef CONFIG_ITRACE

void trace_inst_record(char *log)
{
    strncpy(iringbuf[iringbuf_idx], log, IRINGBUF_INFO_SIZE-1);
    iringbuf[iringbuf_idx][IRINGBUF_INFO_SIZE-1] = '\0';
    iringbuf_idx = iringbuf_idx < IRINGBUF_SIZE-1 ? iringbuf_idx+1 : 0;
    if (first_idx < IRINGBUF_SIZE) first_idx++;
}

void trace_inst_print(int mode)
{
    if (mode == 0) {
        fprintf(f, "\n------------------- iringbuf trace -------------------\n");
        int error_pos = iringbuf_idx - 1 < 0 ? IRINGBUF_SIZE - 1 : iringbuf_idx - 1;
        for (int i = 0; i < first_idx; ++i) {
            if (i == error_pos) {
                fprintf(f, "> ");
            }
            fprintf(f, "%s\n", iringbuf[i]);
        }
        fprintf(f, "------------------- iringbuf trace -------------------\n");
    } else {
        G_DEBUG_WR("\n------------------- iringbuf trace -------------------\n");
        int error_pos = iringbuf_idx - 1 < 0 ? IRINGBUF_SIZE - 1 : iringbuf_idx - 1;
        for (int i = 0; i < first_idx; ++i) {
            if (i == error_pos) {
                G_DEBUG_WR("> ");
            }
            G_DEBUG_WR("%s\n", iringbuf[i]);
        }
        G_DEBUG_WR("------------------- iringbuf trace -------------------\n");        
    }
}

void itrace_init(char *log_path)
{
    assert(log_path != nullptr);
    char path_dir[100];
    init_disasm("riscv64-pc-linux-gnu");
    strncpy(path_dir, log_path, 100);
    f = fopen(strncat(path_dir, "/itrace_log.txt", 100), "w");
    count = 0;
}

void itrace_recorde()
{
    char *p = buf;
    p += snprintf(p, sizeof(buf), "0x%016lx" ":", dut.io_pc);
    memset(p, ' ', 1);
    p += 1;

    uint32_t inst_ = (uint32_t)paddr_read(dut.io_pc, 4);
    
    disassemble(p, buf + sizeof(buf) - p, dut.io_pc, (uint8_t *)&inst_, 4);
    trace_inst_record(buf);
    if (count >= CONFIG_TRACE_START && count < CONFIG_TRACE_END) {
        fprintf(f, "%s\n", buf);
        count++;
    }
}

void itrace_deinit()
{
    if (f != nullptr)
        fclose(f);
}

#endif

static bool ftrace_is_jal(uint32_t inst)
{
    return BITS(inst, 6, 0) == 0b1101111;
}

static bool ftrace_is_jalr(uint32_t inst)
{
    return (BITS(inst, 6, 0) == 0b1100111) && (BITS(inst, 14, 12) == 0b000);
}

// bool ftrace_jalr_ret(uint32_t) {
//     bool success;

// }

static uint64_t jal_addr_calc(uint32_t inst, uint64_t pc) {
    uint32_t i = inst;
    uint64_t imm;
    immJ();
    return imm+pc;
}

static uint64_t jalr_addr_calc(uint32_t inst)
{
    uint32_t i = inst;
    uint64_t imm;
    immI();
    return (imm+gpr(BITS(i, 19, 15))) & ~BITMASK(1);
}

static int ftrace_call_find(uint32_t inst, uint64_t pc) 
{
    uint64_t addr = 0;
    if (ftrace_is_jal(inst)) {
        addr = jal_addr_calc(inst, pc);
    } else if (ftrace_is_jalr(inst)) {
        addr = jalr_addr_calc(inst);
    } else {
        return -1;
    }
    for (int i = 0; i < elf_func_cnt; i++) {
        if (addr == elf_func_syms[i].st_value) {
            return i;
        }
    }
    return -1;
}

static int ftrace_ret_find(uint32_t inst, uint32_t pc)
{
    if (ftrace_is_jalr(inst) && \
    (BITS(inst, 19, 15) == 1) && (BITS(inst, 11, 7) == 0)) {
        for (int i = 0; i < elf_func_cnt; i++) {
            if (pc >= elf_func_syms[i].st_value && \
                pc < elf_func_syms[i].st_value + elf_func_syms[i].st_size) {
                return i;
            }
        }
    }
    return -1;
}

static void print_align(int gap)
{
    for (int i = 0; i < gap; i++) {
        fprintf(ftrace_fp, "  ");
    }
}

#ifdef CONFIG_FTRACE

void trace_ftrace_init(char *log_path, char *elf_file)
{
    assert(log_path != nullptr);
    assert(elf_file != nullptr);
    char path_dir[100];

    strncpy(path_dir, log_path, 100);

    ReaderInit(elf_file);
    ftrace_fp = fopen(strncat(path_dir, "/ftrace_log.txt", 100), "w");
    ftrace_deepth = 0;
}

void trace_ftrace_deinit()
{
    ReaderDeInit();
    fclose(ftrace_fp);
    ftrace_fp = nullptr;
}

void trace_ftrace_print(uint32_t inst, uint64_t pc)
{
    int idx = 0;
    // if (ftrace_printcnt > 300) {
    //     ftrace_printcnt = 0;
    //     fseek(ftrace_fp, 0, SEEK_SET);
    // }
    if ((idx = ftrace_ret_find(inst, pc)) >= 0) {
        ftrace_printcnt++;
        fprintf(ftrace_fp, "0x%lx: ", pc);
        ftrace_deepth--;
        print_align(ftrace_deepth);
        fprintf(ftrace_fp, "ret [%s]\n", ReaderSymbolGetName(&elf_func_syms[idx]));
        fflush(ftrace_fp);
    } else if ((idx = ftrace_call_find(inst, pc)) >= 0) {
        ftrace_printcnt++;
        fprintf(ftrace_fp, "0x%lx: ", pc);
        print_align(ftrace_deepth);
        fprintf(ftrace_fp, "call [%s@0x%lx]\n", \
        ReaderSymbolGetName(&elf_func_syms[idx]), elf_func_syms[idx].st_value);
        ftrace_deepth++;
        fflush(ftrace_fp);
    }
}

#endif
