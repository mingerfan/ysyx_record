#include <csr.h>
#include "local-include/reg.h"
#include "macro.h"
#include <stdint.h>

uint64_t set_bits_from_b(uint64_t a, uint64_t hi, uint64_t lo, uint64_t b) {
    // 创建掩码，用于清除a中的目标位段
    uint64_t mask = ~(((1ul << (hi - lo + 1)) - 1) << lo);
    // 清除a中的目标位
    a &= mask;
    // 将b的相关位移动到正确的位置
    b = (b << lo) & ~mask;
    // 将清除后的a与移位后的b进行'或'操作以设置目标位
    return a | b;
}

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

CsrFunc new_bynum(uint64_t csr_addr) {
    word_t *csr = get_csr_bynum(csr_addr);
    CsrFunc ret = {
        .read = default_read,
        .write = default_write,
        .csr = csr,
    };
    return ret;
}

void mstatus_write(CsrFunc *self, word_t data) {
    // printf("Data: %lx\n", data);
    data = set_bits_from_b(data, 63, 63, 0); // hardware sd to zero
    // printf("Data1: %lx\n", data);
    data = set_bits_from_b(data, 63, 36, 0); // hardware field, MBE, SBE to zero
    // printf("Data2: %lx\n", data);
    data = set_bits_from_b(data, 35, 32, 0xA);  // SXL = 10b, UXL = 10b
    // printf("Data3: %lx\n", data);
    data = set_bits_from_b(data, 31, 23, 0); // hardware WPRI field to zero
    // printf("Data4: %lx\n", data);
    // data = set_bits_from_b(data, 22, 13, 0); // hardware to zero
    // printf("Data5: %lx\n", data);
    data = set_bits_from_b(data, 12, 11, 0x3); // mpp = 11b
    // printf("Data6: %lx\n", data);
    data = set_bits_from_b(data, 10, 4, 0);
    data = set_bits_from_b(data, 2, 2, 0);
    data = set_bits_from_b(data, 0, 0, 0);
    // printf("Data End: %lx\n", data);
    *self->csr = data;
}

CsrFunc new_mstatus() {
    word_t *csr = get_csr("mstatus");
    CsrFunc ret = {
        .read = default_read,
        .write = mstatus_write,
        .csr = csr,
    };
    return ret;
}