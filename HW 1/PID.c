#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

//Problem 1: Get PID
//Problem 3: Get PID and stay in infinite loop
int main(){
    printf("pid: %i\n", getpid());
    while(1);
}