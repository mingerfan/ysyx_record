#include <stdint.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <NDL.h>

int main() {
    uint32_t t, last_t;
    t = NDL_GetTicks();
    last_t = t;
    for (int i = 0; i < 10; i++) {
        printf("curtime: %d\n", t);
        while (1) {
            t = NDL_GetTicks();
            if (t-last_t >= 500) {
                last_t = t;
                break;
            }
        }
    }
}