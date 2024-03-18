#include <csr.h>
#include "local-include/reg.h"

word_t default_read(CsrFunc *self) {
    return *self->csr;
}

void default_write(CsrFunc *self, word_t data) {
    *self->csr = data;
}

CsrFunc new(const char *s) {
    word_t *csr = get_csr(s);
    CsrFunc ret = {
        .read = default_read,
        .write = default_write,
        .csr = csr,
    };
    return ret;
}

