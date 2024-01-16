#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

int main() {
    struct timeval tv, last_tv;
    assert(gettimeofday(&tv, (void*)0) == 0);
    memcpy(&last_tv, &tv, sizeof(struct timeval));
    for (int i = 0; i < 10; i++) {
        printf("sec: %ld, usec: %ld\n", tv.tv_sec, tv.tv_usec);
        while (1) {
            assert(gettimeofday(&tv, (void*)0) == 0);
            if ((tv.tv_sec - last_tv.tv_sec)*1000 + (tv.tv_usec - last_tv.tv_usec)/1000 >= 500) {
                memcpy(&last_tv, &tv, sizeof(struct timeval));
                break;
            }
        }
    }
}