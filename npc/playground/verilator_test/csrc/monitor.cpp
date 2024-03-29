#include <monitor.h>
#include <stdlib.h>
#include <getopt.h>
#include <npc_memory.h>
#include <sdb.h>
#include <mytrace.h>
#include <debug.h>
#include <difftest.h>
#include <regs.h>

// void init_disasm(const char *triple);

static char *bin_path = NULL;
static char *log_path = NULL;
static char *elf_path = NULL;
static char *nemu_so_path= NULL;
bool use_ftrace = false;
bool use_difftest = false;
int ftrace_arg_state = 0;

static int parse_args(int argc, char *argv[])
{
    const struct option table[] = {
        {"batch" , required_argument, NULL, 'b'},
        {"lpath" , required_argument, NULL, 'l'},
        {"ftrace", required_argument, NULL, 'f'},
        {"diff"  , required_argument, NULL, 'd'},
        {0       , 0                , NULL,  0 },
    };
    int o;
    while ((o = getopt_long(argc, argv, "-bl:f:d:", table, NULL)) != -1) {
        switch (o) {
            case 'b': sdb_set_batch_mode(); break;
            case 'l': log_path = optarg; break;
            case 'f': {
                printf("Turn on use ftrace: %s\n", optarg);
                if (ftrace_arg_state == 0) {
                trace_ftrace_start(optarg);
                use_ftrace = true;
                ftrace_arg_state++;
                } else {
                trace_ftrace_add(optarg);
                ftrace_arg_state++;
                }
                break;
            }
            case 'd': nemu_so_path = optarg; use_difftest= true; break;
            case 1: bin_path = optarg; return 0;
            default: 
            return 0;
        }
    }
    return 0;
}

void monitor_init(int argc, char *argv[])
{
    parse_args(argc, argv);

    mem_init(bin_path);

    init_sdb();

    itrace_init(log_path);
    
    if (use_ftrace) {
        trace_ftrace_init();
    }

    if (use_difftest) {
        extern long npcmem_img_size;
        CPU_state cpu;
        rr_wrap(&cpu);
        init_difftest(nemu_so_path, npcmem_img_size, 1234, &cpu);
    }
}

void monitor_deinit()
{
    itrace_deinit();
}