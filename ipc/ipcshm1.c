#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>

#define MAPPED_SIZE 128

int main(int argc, char* argv[])
{
	// Create and open a shared memory segment
	int shmfd = shm_open("/CS3841MEMORY", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(shmfd == -1) {
		printf("COULD NOT OPEN SHARED MEMORY SEGMENT\n");
		exit(EXIT_FAILURE);
	}
	ftruncate(shmfd, MAPPED_SIZE);
	void* mapped_space = mmap(NULL, MAPPED_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if(mapped_space == MAP_FAILED) {
		printf("COULD NOT MMAP\n");
		exit(EXIT_FAILURE);
	}

	pid_t pid = fork();
	if(pid < 0) {
		printf("COULD NOT FORK\n");
		exit(EXIT_FAILURE);
	} else if(pid == 0) {
		// Child writes to shared memory segment
		strcpy(mapped_space, "HELLO");
		munmap(mapped_space, MAPPED_SIZE);
		close(shmfd);
		return 0;
	} else {
		// Wait for child to finish
		wait(0);

		// Parent reads from shared memory segment
		printf("Parent reads %s from shared mapped segment\n", (char*)mapped_space);
		munmap(mapped_space, MAPPED_SIZE);
		close(shmfd);
	}

	return 0;
}
