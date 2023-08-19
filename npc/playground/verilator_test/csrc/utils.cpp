#include "utils.h"
#include <sys/time.h>

uint64_t boot_time = 0;

uint64_t get_time() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    uint64_t us = tv.tv_sec * 1000000 + tv.tv_usec;
    if (boot_time == 0) boot_time = us;
    return us - boot_time;
}