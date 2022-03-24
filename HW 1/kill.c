#include <stdio.h>
#include <signal.h>

//Problem 3: Kill process given PID
int main(){
    printf("Enter PID to kill: ");
    int PID;
    scanf("%i", &PID);
    kill(PID, SIGKILL);
}