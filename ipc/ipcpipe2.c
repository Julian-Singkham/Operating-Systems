#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int pipe_to_child[2];
	int pipe_from_child[2];
	pid_t pid;

	if (pipe(pipe_to_child) == -1) {
		printf("PIPE FAILURE\n");
		exit(EXIT_FAILURE);
	}
	if (pipe(pipe_from_child) == -1) {
		printf("PIPE FAILURE\n");
		exit(EXIT_FAILURE);
	}

	pid = fork();
	if (pid == -1) {
		printf("FORK FAILURE\n");
		exit(EXIT_FAILURE);
	} else 	if (pid == 0) {
		/* Close unused pipe ends */
		close(pipe_to_child[1]);
		close(pipe_from_child[0]);
		
		/* Write to parent */
		write(pipe_from_child[1], "CHELLO", 6);
		close(pipe_from_child[1]);

		/* Read from parent */
		char data[32];
		read(pipe_to_child[0], data, sizeof(data));
		printf("Child received %s from parent\n", data);
		close(pipe_to_child[0]);
		return 0;
	} else {
		/* Close unused pipe ends */
		close(pipe_to_child[0]);
		close(pipe_from_child[1]);

		/* Write to child */
		write(pipe_to_child[1], "PHELLO", 6);
		close(pipe_to_child[1]);

		/* Read from child */
		char data[32];
		read(pipe_from_child[0], data, sizeof(data));
		printf("Parent recieved %s from child\n", data);
		close(pipe_from_child[0]);
		wait(0);
	}
}
