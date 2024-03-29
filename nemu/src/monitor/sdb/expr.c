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
#include <memory/vaddr.h>
// char expr_err_buf[50][100];
// int expr_err_index = 0;
// void exprp() {
//   if (expr_err_index < 50) {
//     ++expr_err_index;
//   }
// }

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  NUM,
  HEX_NUM,
  REG,
  DEREF,
  TK_NEQ,
  AND,
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
  {"0x[0-9A-Fa-f]+", HEX_NUM},// hex start with 0x
  {"-{0,1}[0-9]+", NUM},      // numbers
  {"-", '-'},                 // sub
  {"\\*", '*'},               // multiply
  {"/", '/'},                 // left slash
  {"\\(", '('},               // left bracket
  {"\\)", ')'},               // right bracket
  {"\\$[a-zA-Z0-9]+", REG},           // reg start with $
  // DEREF the same *
  {"!=", TK_NEQ},             // not equal to
  {"&&", AND}                 // and
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

#define TOKEN_MAX 200
static Token tokens[TOKEN_MAX] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

void token_str_cpy(char *start, int len)
{
  assert(len < 32);
  for (int j = 0; j < len; ++j) {
    tokens[nr_token].str[j] = *(start+j);
  }
  tokens[nr_token].str[len] = '\0';
}

bool token_deref_judge(int idx) {
  bool res = tokens[idx].type != NUM ;
  res &= tokens[idx].type != HEX_NUM;
  res &= tokens[idx].type != REG;
  res &= tokens[idx].type != ')';
  return res;
}

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

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

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
          token_str_cpy(substr_start, substr_len);
          break;

          case(HEX_NUM):
          tokens[nr_token].type = HEX_NUM;
          token_str_cpy(substr_start, substr_len);
          break;

          case(REG):
          tokens[nr_token].type = REG;
          token_str_cpy(substr_start+1, substr_len-1);
          break;

          default: tokens[nr_token].type = rules[i].token_type;
        }
        if (tokens[nr_token].type == '*' && (nr_token == 0 || token_deref_judge(nr_token-1))) {
          tokens[nr_token].type = DEREF;
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
  bool not_end = false;
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
    } else if (cnt == 0 && i != q) {
      not_end = true;
    }
  }
  if (cnt > 0) {
    *err = true;
    return false;
  }
  if (tokens[p].type == '(' && tokens[q].type == ')' && !not_end) {
    return true;
  }
  return false;
}

int find_main_op(int p, int q) {
  #define op_max_pros 5
  #define op_max_num 5
  int ops[op_max_pros][op_max_num] = {{AND},{TK_NEQ, TK_EQ}, {'+', '-'}, {'*','/'}, {DEREF}};
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
        if (j/op_max_num <= cur_main_op_idx/op_max_num) {
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

word_t eval(int p, int q) {
  bool is_pat;
  word_t num = 0;
  int op;
  word_t val1, val2;
  traceback.p = p;
  traceback.q = q;
  bool success;

  if (traceback.err) {
    return 0;
  }
  else if (p > q) {
    traceback.err = true;
    return 0;
  }
  else if (p == q) {
    // sprintf(expr_err_buf[expr_err_index], "index: %d, str: %s\n", p, tokens[p].str);
    // exprp();
    if (tokens[p].type == NUM) {
      assert(sscanf(tokens[p].str, "%lu", &num) == 1);
    }
    else if (tokens[p].type == HEX_NUM)
    {
      assert(sscanf(tokens[p].str, "%lx", &num) == 1);
    }
    else if (tokens[p].type == REG) {
      num = isa_reg_str2val(tokens[p].str, &success);
      if (!success) {
        traceback.err = true;
        return 0;
      }
    }
    return num;
  }
  else if (check_parentheses(p, q, &is_pat)){
    if (is_pat) {
      traceback.err = true;
      return 0;
    }
    return eval(p+1, q-1);
  }
  else {
    op = find_main_op(p, q);
    if (tokens[op].type != DEREF) {
      val1 = eval(p, op - 1);
    }
    else {
      val1 = 0;
    }
    val2 = eval(op + 1, q);

    // sprintf(expr_err_buf[expr_err_index], "op: %c\n", tokens[op].type);
    // exprp();
    // sprintf(expr_err_buf[expr_err_index], "val1: %lu\n", val1);
    // exprp();
    // sprintf(expr_err_buf[expr_err_index], "val2: %lu\n", val2);
    // exprp();

    switch (tokens[op].type) {
      case('+'): return val1 + val2;
      case('-'): return val1 - val2;
      case('*'): return val1 * val2;
      case('/'): return (sword_t)val1 / (sword_t)val2;
      case(TK_EQ): return val1 == val2;
      case(TK_NEQ): return val1 != val2;
      case(AND): return val1 && val2;
      case(DEREF): return vaddr_read(val2, 4);
      default: assert(0);
    }
  }
  return 0;
}

word_t expr(char *e, bool *success) {
  word_t result;
  // expr_err_index = 0;
  *success = true;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  traceback.p = 0;
  traceback.q = nr_token-1;
  traceback.err = false;
  result = eval(0, nr_token-1);

  if (traceback.err) {
    printf("ERROR from %d to %d\n ", traceback.p, traceback.q);
    *success = false;
    return 0;
  }

  return result;
}
