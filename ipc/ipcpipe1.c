#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int pipefd[2];
	pid_t pid;

	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else 	if (pid == 0) {
		/* Close unused write end */
		close(pipefd[1]);
		char data[32];
		read(pipefd[0], data, sizeof(data));
		printf("Child received %s from parent\n", data);
		close(pipefd[0]);
		return 0;
	} else {
		/* Close unused read end */
		close(pipefd[0]);
		write(pipefd[1], "HELLO", 5);
		close(pipefd[1]);
		wait(0);
		exit(EXIT_SUCCESS);
	}
}
