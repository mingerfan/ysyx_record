/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/vaddr.h>
#include <stdio.h>
#include "../local-include/reg.h"

static int is_batch_mode = false;
extern NEMUState nemu_state;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  cpu_exec(1);
  return 0;
}

static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_mt(char *args);
static int cmd_mtt(char *args);
static int cmd_d(char *args);
static int cmd_w(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Run Instruction by step", cmd_si },
  { "info", "Get cpu info", cmd_info },
  { "x", "Get memory data", cmd_x },
  { "mt", "Match try", cmd_mt },
  { "mtt", "Match try test", cmd_mtt },
  { "d", "Delete watchpoint", cmd_d },
  { "w", "Add watchpoint", cmd_w }
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_info(char *args) {
  char *arg = strtok(NULL, " ");
  int i = 0;
  if (arg == NULL) {

  }
  else if (strcmp(arg, "r") == 0) {
    for (i = 0; i < 32; ++i) {
      printf("%3s\t=\t0x%08lx\n", reg_name(i, 0), gpr(i));
    }
  }
  else if (strcmp(arg, "w") == 0) {
    info_wp();
  }
  return 0;
}

static int cmd_x(char *args) {
  char *arg = strtok(NULL, " ");
  int i = 0, j = 0;
  int times;
  bool success;
  word_t start_addr;
  uint16_t state = 0;
  for (i = 0; ;++i) {
    if (arg == NULL) {
      break;
    }
    else if (state == 0) {
      if (sscanf(arg, "%d", &times) != 1) {
        return 0;
      }
      state = 1;
    }
    else if (state == 1) {
      printf("arg: %s\n", arg);
      start_addr = expr(arg, &success);
      start_addr = start_addr-start_addr%4;
      for (j = 0; j < times; ++j) {
        printf("0x%016lx\t=\t%08lx\n", start_addr+j*4, vaddr_read(start_addr+j*4, 4));
      }
      return 0;
    }
  }
  return 0;
}

static int cmd_mt(char *args) {
  word_t expr(char *e, bool *success);
  bool success;
  word_t result;
  result = (word_t)expr(args, &success);
  printf("expr result: %lu\n", result);
  return 0;
}


static int cmd_mtt(char *args) {
  word_t result = 0, result1 = 0;
  bool success;
  char c_read;
  char buf[65536] = {};
  int index = 0;
  FILE *f = fopen("/home/xs/ysyx/ysyx-workbench/nemu/tools/gen-expr/input", "r");
  int cnt = 0;
  while (fscanf(f, "%lu ", &result) != EOF) {
    index = 0;
    while(1) {
      c_read = fgetc(f);
      if (c_read == '\n') {
        buf[index] = '\0';
        break;
      }
      buf[index++] = c_read;
    }
    result1 = (word_t)expr(buf, &success);
    if (success && result == result1) {
      // printf("Pass!\n");
    }
    else {
      ++cnt;
      printf("buf is %s\n", buf);
      printf("Failed!\t");
      printf("out: %lu, should be: %lu\n\n", result1, result);
    }
  }
  printf("Failed count: %d\n", cnt);
  return 0;
}

static int cmd_d(char *args) {
  char *arg = strtok(NULL, " ");
  int no;
  WP *p;
  assert(sscanf(arg, "%d", &no) == 1);
  p = scan_wp_idx(no);
  free_wp(p);
  return 0;
}

static int cmd_w(char *args) {
  char *arg = strtok(NULL, "");
  printf("arg:%s\n", arg);
  WP *p;
  if (arg) {
    p = new_wp();
    bind_exprs_wp(p, arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (args)
          Log("Command Args: %s", args);
        else
          Log("Command Args: %s", "(NULL)");

        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
