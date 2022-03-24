#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>


//Problem 2: Get Time in nanoseconds
int main(){
    int64_t time = 0;
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    time = tp.tv_nsec;
    printf("Time: %li ns\n", time);
}


 