#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
	int myfile = open("MYFILE", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	
	pid_t pid = fork();
	if(pid < 0) {
		printf("ERROR: COULD NOT FORK\n");
		exit(EXIT_FAILURE);
	} else if(pid == 0) {
		char* data = "HELLO";
		write(myfile, data, strlen(data));
		close(myfile);
		printf("CHILD WROTE %s\n", data);
		return 0;
	} else {
		wait(0);
		lseek(myfile, 0, SEEK_SET);
		char data[32];
		read(myfile, data, sizeof(data));
		printf("Parent received %s from child\n", data);
		close(myfile);
	}
	return 0;
}
