#include <klib_tool.h>
#include <klib.h>

static int klib_abs(int a)
{
  return a > 0 ? a : -a;
}

char* Int2Char(char *buf, int num) {
  int idx = 0;
  char tmp;
  if (num < 0) {
    *buf++ = '-';
  }
  if (num == 0) {
    *buf++ = '0';
    return buf;
  }
  for (idx = 0; num != 0; num /= 10) {
    buf[idx++] = (char)(klib_abs(num%10) + 48);
  }
  idx--;
  int idx_tmp = idx;
  for (int i = 0; i < idx; ++i) {
    tmp = buf[i];
    buf[i] = buf[idx];
    buf[idx--] = tmp;
  }
  return buf+idx_tmp+1;
}

// Decode the Descriptor and return the new pointer after decoding
const char* PrintDesDec(prtDes *p, const char *des) {
    // jump % and don't care %% now
    switch (*(des+1)) {
        case 'd':
        p->Spec = PRINT_INT; break;
        case 's':
        p->Spec = PRINT_STR; break;
        default:
        p->Spec = PRINT_FAILED;
    }
    return des + 2;
}

// similar to strcpy but return the end of dest
char* StrCpyTool(char *dst, const char *src) {
    assert((dst!=NULL)&&(src!=NULL));
    while ((*dst++ = *src++) != '\0');
    return dst--;
}

int StrCnt(char *s) {
    assert(s!=NULL);
    int cnt = 0;
    while (*s != '\0') {
        ++cnt;
        ++s;
    }
    return cnt;
}