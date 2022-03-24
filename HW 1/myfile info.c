#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

//Problem 7: Display the information of MYFILE
int main(){
    struct stat info;
    stat("MYFILE", &info);

    printf("MYFILE Info:\n Size:%lli\n, Inode:%li\n", &info.st_size, &info.st_ino);
}