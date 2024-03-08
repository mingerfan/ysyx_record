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
#include <bindings.h>

static char buf[128];
extern TOP_NAME dut;
static FILE *f = nullptr;
static int count = 0;

char iringbuf[IRINGBUF_SIZE][IRINGBUF_INFO_SIZE] = {};
static int iringbuf_idx = 0;
static int first_idx = 0;

#ifdef CONFIG_FTRACE
extern bool use_ftrace;
static char *log_file = NULL;
#endif

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
    // if (count >= CONFIG_TRACE_START && count < CONFIG_TRACE_END) {
    //     fprintf(f, "%s\n", buf);
    //     count++;
    // }
}

void itrace_deinit()
{
    if (f != nullptr)
        fclose(f);
}

#endif

void trace_ftrace_start(char *file)
{
#ifdef CONFIG_FTRACE
    char *last_slash = strrchr(file, '/');
    int dir_len = last_slash - file + 1;
    log_file = (char*)malloc(dir_len + 30);
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
        print_stack(log_file);
    }
#endif
}

void trace_etrace_ecall(CPU_state cpu, word_t NO) {
#ifdef CONFIG_ETRACE
    printf("ECALL! mepc: 0x%016lx, mtvec: 0x%016lx, NO: %ld, pc: 0x%08lx\n", cpu.csrs.mepc, cpu.csrs.mtvec, NO, cpu.pc);
#endif
}



