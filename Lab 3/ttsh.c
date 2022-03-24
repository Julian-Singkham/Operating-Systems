/**
  ***********************************************************************************
  * @file     : Main.c
  * @brief    : Main program body
  *           : Lab 3: Teeny Tiny Shell
  *           : CS-3841/021 
  * @date     : OCT 01 2020
  * @author   : Julian Singkham
  * @coauthor : Prof Lembke (Created initial starting code)
  ***********************************************************************************
  * @attention
  *
  * This program creates a limited Bash style terminal.
  *
  ***********************************************************************************
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h> 

#define INPUT_MAX 256
#define CMD_MAX 5
#define ARGUMENT_MAX 10

/* read_cmd_string()
 *     Reads a line of text from the user
 *         Parameters:  dest - target location for the input
 *         Returns: int - 0 on success or 
 *                        -1 on error
 */
int read_cmd_string(char dest[INPUT_MAX]){
	// Read user input
	if(fgets(dest, INPUT_MAX, stdin) == NULL) {
		fprintf(stderr, "Unable to read user input\n");
		return -1;
	}

	// Remove trailing return character
	int len = strlen(dest);
	if(dest[len-1] == '\n') {
		dest[len - 1] = '\0';
	}

	return 0;
}

/* parse_commands()
 *     Reads a line of text from the user
 *         Parameters:  input - string containing user input
 *                      cmd_strs - the target array for command strings
 *         Returns: int - The number of commands found or
 *                        -1 on error
 */
int parse_commands(char input[INPUT_MAX], char cmd_strs[CMD_MAX][INPUT_MAX])
{
	// Chop the input into command strings
	int cmd_count = 0;
	char* cmd_ptr = strtok(input, ";");
	while(cmd_ptr) {
		if(cmd_count >= CMD_MAX) {
			fprintf(stderr, "Too many commands\n");
			return -1;
		}
		strncpy(cmd_strs[cmd_count], cmd_ptr, INPUT_MAX);
		cmd_count++;
		cmd_ptr = strtok(NULL, ";");
	}

	return cmd_count;
}

int main(){
	char user_input[INPUT_MAX];
	while(1){
		printf("$> ");
		//Check if user input is useable and if the input is 'quit'
		if(read_cmd_string(user_input) == -1) {
			return 1; //FAILURE
		}
		if (strcmp(user_input, "quit") == 0){
			return 0;
		}

		//Chop the input into command strings
		char cmd_strs[CMD_MAX][INPUT_MAX];
		int cmd_count = parse_commands(user_input, cmd_strs);
		if(cmd_count == -1) {
			return 1; // Failure
		}
		
		//Parse command into arguments and run
		char *arg_strs[ARGUMENT_MAX];
		for(int i = 0; i < cmd_count; i++) {
			//Chop command into arguments
			int arg_count = 0;
			char* arg_ptr = strtok(cmd_strs[i], " ");
			while(arg_ptr) {
				if(arg_count >= ARGUMENT_MAX) {
					fprintf(stderr, "Tooo many arguments\n");
					return -1;
				}
				arg_strs[arg_count] = arg_ptr;
				arg_count++;
				arg_ptr = strtok(NULL, " ");
			}
			
			pid_t  pid;
			pid = fork();
			//Chile process
			if (pid == -1){
				printf("Failed to create child process.\n");
			}
			if(pid == 0){
				if(execvp(arg_strs[0], arg_strs) == -1){
					printf("Failed to run command.\n");
				}
				return 0;
			}
			//Parent process
			else{
				wait(0);
			}
		}
	}
	return 0;
}
