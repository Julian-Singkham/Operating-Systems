#include <fcntl.h>
#include <string.h>

//Problem 4: Create MYFILE
int main(){
    char *input = "HELLO_LINUX";
    int file = open("MYFILE", O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    write(file, input, strlen(input));
    close(file);
}