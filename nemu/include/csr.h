#pragma once
#include <common.h>

typedef struct CsrFunc CsrFunc;

typedef word_t (*ReadFunc)(CsrFunc *self);
typedef void (*WriteFunc)(CsrFunc *self, word_t);

typedef struct CsrFunc
{
    /* data */
    ReadFunc read;
    WriteFunc write;
    word_t *csr;
} CsrFunc;

word_t default_read(CsrFunc *self);
void default_write(CsrFunc *self, word_t data);
CsrFunc new(const char *s);
CsrFunc new_bynum(uint64_t csr_addr);
CsrFunc new_mstatus();