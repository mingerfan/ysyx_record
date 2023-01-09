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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned int result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int ex_index = 0;
int ex_deep = 0;

#define MAX_NUM ((uint32_t)(-1)/2 - 1)/100000
#define CHOOSE(x) (rand()%x)
#define OVERFLOW_CHECK(x) assert(ex_index + x < 65535)

void gen_num() {
  ++ex_deep;
  char temp_buf[12];
  OVERFLOW_CHECK(11); // consider nagative sign
  int32_t num = rand() % MAX_NUM - MAX_NUM/2;
  sprintf(temp_buf, "%d", num);
  for (int i = 0; i < 11; ++i) {
    if (temp_buf[i] == '\0') {
      break;
    } 
    else {
      *(buf + ex_index) = temp_buf[i];
      ++ex_index;
    }
  }
}

void gen(char *s) {
  ++ex_deep;
  OVERFLOW_CHECK(strlen(s));
  strcpy(buf+ex_index, s);
  ex_index += strlen(s);
}

void gen_rand_op() {
  ++ex_deep;
  char ops[][10] = {" + ", " - ", " * ", " / "};
  int size = sizeof(ops) / sizeof(*ops);
  int idx = CHOOSE(size);
  OVERFLOW_CHECK(strlen(ops[idx]));
  strcpy(buf+ex_index, ops[idx]);
  ex_index += strlen(ops[idx]);
}

static void gen_rand_expr() {
  if (ex_deep > 10) {
    gen_num();
    return;
  }
  switch (CHOOSE(3))
  {
  case(0):
    gen_num();
    break;
  case(1):
    gen("(");
    gen_rand_expr();
    gen(")");
    break;
  default:
    gen_rand_expr();
    gen_rand_op();
    gen_rand_expr();
    break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    ex_index = 0;
    gen_rand_expr();
    buf[ex_index] = '\0';

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
