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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void wp_data_init(WP *p) {
  return;
}

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_data_init(wp_pool+i);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
  assert(free_);
  WP *tp_free = free_;
  free_ = free_->next;
  tp_free->next = head;
  head = tp_free;
  return tp_free;
}

void free_wp(WP *wp) {
  WP *tp = head;
  WP *last = tp;
  if (!wp) {
    return;
  }
  if (head == wp) {
    head = wp->next;
  }
  else {
    while (1)
    {
      if (!tp) {
        return;
      }
      else if (tp && wp == tp) {
        break;
      }
      last = tp;
      tp = tp->next;
    }
    last->next = wp->next;
  }
  wp->next = free_;
  free_ = wp;
}