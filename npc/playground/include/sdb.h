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

#ifndef __WP_H__
#define __WP_H__

#include <stdint.h>
#include <stdbool.h>

void init_sdb();
void sdb_mainloop();

#ifdef __cplusplus
extern "C" {
#endif

#define NR_WP 32
#define WP_BUF_MAX 100

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char buf[WP_BUF_MAX];
  bool isEnb;
} WP;

uint64_t expr(char *e, bool *success);

WP* new_wp();
void free_wp(WP *wp);
WP* scan_wp();
void info_single_wp(WP *p);
void info_wp();
WP* scan_wp_idx(int idx);
WP* scan_wp_cb(bool(*f)(WP*));
void bind_exprs_wp(WP *p, char *s);

#ifdef __cplusplus
}
#endif

#endif
