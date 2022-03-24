#include <fcntl.h>
#include <string.h>

//Problem 6: Write my name 100 spaces away in MYFILE
int main(){
    char *input = "Julian Singkham";
    int file = open("MYFILE", O_CREAT | O_RDWR);
    lseek(file, 100, SEEK_SET);
    write(file, input, strlen(input));
    close(file);
}