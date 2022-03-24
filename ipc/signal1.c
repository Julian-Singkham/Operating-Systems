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
	signal(SIGINT, signal_handler);

	while(1) {
		printf("sleeping...\n");
		sleep(1);
	}
}
