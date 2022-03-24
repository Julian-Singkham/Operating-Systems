#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <mqueue.h>

typedef struct message {
	int message_id;
	char string[10];
} message;

int main(int argc, char* argv[])
{
	// Create attributes for new queue
	struct mq_attr queue_attr;
	queue_attr.mq_flags = 0;    // Ignored by kernel
	queue_attr.mq_maxmsg = 10;  // Max messages the queue supports
	queue_attr.mq_msgsize = sizeof(message);
	queue_attr.mq_curmsgs = 0;  // Not used

	// Create and open a queue
	mqd_t mqdes = mq_open("/CS3841QUEUE", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &queue_attr);
	if(mqdes == -1) {
		printf("COULD NOT OPEN QUEUE\n");
		exit(EXIT_FAILURE);
	}

	pid_t pid = fork();
	if(pid < 0) {
		printf("COULD NOT FORK\n");
		exit(EXIT_FAILURE);
	} else if(pid == 0) {
		// Child receives message from parent
		message from_parent;
		mq_receive(mqdes, (char*)&from_parent, sizeof(message), NULL);
		printf("Child got %d: %s from parent\n", from_parent.message_id, from_parent.string);
		mq_close(mqdes);
		return 0;
	} else {
		// Parent sends messages to child
		message to_child;
		for(int i = 0; i < 10; i++) {
			to_child.message_id = i;
			strcpy(to_child.string, "HELLO");
			mq_send(mqdes, (char*)&to_child, sizeof(message), 0);
		}
		wait(0);
		mq_close(mqdes);
	}

	return 0;
}
