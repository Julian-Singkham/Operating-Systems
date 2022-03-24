#include <fcntl.h>
#include <string.h>

//Problem 5: Append to MYFILE
int main(){
    char *input = "LINUX_HELLO";
    int file = open("MYFILE", O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
    write(file, input, strlen(input));
    close(file);
}