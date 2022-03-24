#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    nice(10);
    volatile int temp = 0;
    printf("%ld\n", getpid());
    for(int i = 0; i < 9999999999; i++){
        temp++;
    }
}