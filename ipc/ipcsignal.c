#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void signal_handler(int sig)
{
	pid_t pid = getpid();
	printf("Process %d received signal %d\n", pid, sig);
}

int main(int argc, char* argv[])
{
	pid_t pid = fork();
	signal(SIGUSR1, signal_handler);
	if(pid < 0) {
		printf("COULD NOT FORK\n");
		exit(EXIT_FAILURE);
	} else if(pid == 0) {
		pid_t parent_pid = getppid();
		printf("Sending SIGUSR1(%d) to %d\n", SIGUSR1, parent_pid);
		kill(parent_pid, SIGUSR1);
		return 0;
	} else {
		wait(0);
	}
}
