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

#include <sdb.h>
#include "stdlib.h"
#include "stdbool.h"
#include <debug.h>
#include <string.h>

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void wp_data_init(WP *p) {
  p->isEnb = true;
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

static void info_head_wp() {
  printf("%-8s%-18s%-5s%-15s\n", "NUM", "TYPE", "ENB", "EXPRESSION");
}

static void info_data_wp(WP *p) {
  printf("%-8d%-18s%-5s%-s\n", p->NO, "hw watchpoint", p->isEnb? "y":"n", p->buf);
}

void info_single_wp(WP *p) {
  info_head_wp();
  info_data_wp(p);
}

void info_wp() {
  info_head_wp();
  WP *tp = head;
  while (tp)
  {
    info_data_wp(tp);
    tp = tp->next;
  }
} 

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
  assert(free_);
  WP *tp_free = free_;
  free_ = free_->next;
  tp_free->next = head;
  head = tp_free;
  wp_data_init(tp_free);
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

void bind_exprs_wp(WP *p, char *s) {
  strcpy(p->buf, s);
}

WP* scan_wp_cb(bool(*f)(WP*)) {
  WP *tp = head;
  while (tp)
  {
    if (f(tp)) {
      return tp;
    }
    tp = tp->next;
  }
  return NULL;
}

WP* scan_wp() {
  WP *tp = head;
  bool success;
  while (tp)
  {
    if (tp->isEnb && !expr(tp->buf, &success) == false && success) {
      return tp;
    }
    tp = tp->next;
  }
  return NULL;
}

WP* scan_wp_idx(int idx) {
  WP *tp = head;
  while (tp)
  {
    if (tp->NO == idx) {
      return tp;
    }
    tp = tp->next;
  }
  return NULL;
}