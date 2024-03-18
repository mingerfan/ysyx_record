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


