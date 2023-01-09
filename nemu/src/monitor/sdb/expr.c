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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  NUM,
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},          // spaces
  {"\\+", '+'},               // plus
  {"==", TK_EQ},              // equal
  {"-{0,1}[0-9]+", NUM},      // numbers
  {"-", '-'},                 // sub
  {"\\*", '*'},               // multiply
  {"/", '/'},                 // left slash
  {"\\(", '('},               // left bracket
  {"\\)", ')'},               // right bracket
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case(TK_NOTYPE): 
          --nr_token;
          break;

          case(NUM):
          tokens[nr_token].type = NUM;
          assert(substr_len <= 32);
          for (int j = 0; j < substr_len; ++j) {
            tokens[nr_token].str[j] = *(substr_start+j);
          }
          break;

          default: tokens[nr_token].type = rules[i].token_type;
        }
        ++nr_token;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q, bool *err)
{
  int cnt = 0;
  *err = false;
  for (int i = p; i <= q; ++i) {
    if (tokens[i].type == '(') {
      ++cnt;
    }
    else if (tokens[i].type == ')') {
      --cnt;
    }
    if (cnt < 0) {
      *err = true;
      return false;
    }
  }
  if (cnt > 0) {
    *err = true;
    return false;
  }
  if (tokens[p].type == '(' && tokens[q].type == ')') {
    return true;
  }
  return false;
}

int find_main_op(int p, int q) {
  #define op_max_pros 3
  #define op_max_num 5
  int ops[op_max_pros][op_max_num] = {{'+', '-'}, {'*','/'}};
  int cnt = 0;
  int cur_main_op_pos = 0;
  int cur_main_op_idx = op_max_pros * op_max_num - 1; // default: highest priority

  for (int i = p; i <= q; ++i) {
    if (tokens[i].type == '(') {
      ++cnt;
    }
    else if (tokens[i].type == ')') {
      --cnt;
    }
    if (cnt != 0) {
      continue;
    }
    for (int j = 0; j < op_max_pros*op_max_num; ++j) {
      if (*((*ops)+j) == tokens[i].type) {
        if (j/op_max_pros <= cur_main_op_idx/op_max_pros) {
          cur_main_op_pos = i;
          cur_main_op_idx = j;
        }
      }
    }
  }
  return cur_main_op_pos;
  #undef op_max_pros
  #undef op_max_num
}

struct eval_traceback {
  int p;
  int q;
  bool err;
} traceback = {
  .p = 0,
  .q = 0,
  .err = false
};

int32_t eval(int p, int q) {
  bool is_pat;
  int num;
  int op;
  int32_t val1, val2;

  if (traceback.err) {
    return 0;
  }
  else if (p > q) {
    traceback.err = true;
    return 0;
  }
  else if (p == q) {
    printf("num is: %s\n", tokens[p].str);
    assert(sscanf(tokens[p].str, "%d", &num) == 1);
    return num;
  }
  else if (check_parentheses(p, q, &is_pat)){
    if (is_pat) {
      traceback.err = true;
      return 0;
    }
    traceback.p = p+1;
    traceback.q = q-1;
    return eval(p+1, q-1);
  }
  else {
    traceback.p = p;
    traceback.q = q;
    op = find_main_op(p, q);
    val1 = eval(p, op - 1);
    val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case('+'): return val1 + val2;
      case('-'): return val1 - val2;
      case('*'): return val1 * val2;
      case('/'): return val1 / val2;
      
      default: assert(0);
    }
  }
  return 0;
}

word_t expr(char *e, bool *success) {
  word_t result;
  *success = true;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  traceback.p = 0;
  traceback.q = nr_token-1;
  traceback.err = false;
  printf("nr_token: %d\n", nr_token);
  result = eval(0, nr_token-1);

  if (traceback.err) {
    printf("ERROR from %d to %d\n ", traceback.p, traceback.q);
    *success = false;
    return 0;
  }

  return result;
}
