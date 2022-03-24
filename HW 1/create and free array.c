//#include <sys/types.h>
//#include <sys/stat.h>
//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <inttypes.h>

//Problem 9: Create an array and dynamically allocate memory. Aftwards free it.
int main(){
    int *arr = (int *)malloc(5*sizeof(int));//Array of 5 ints

    for (int i = 0; i <  5; i++) {
         *(arr + i) = i; 
    }

    for (int i = 0; i < 5; i++) {
         printf("%d ", *(arr + i));
    }
    free(arr);
}