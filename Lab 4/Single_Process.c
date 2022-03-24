/**
  ***********************************************************************************
  * @file   : Single_Process.c
  * @brief  : Main program body
  *         : Lab 4: Shared Memory Matrix Addition
  *         : CS-3841/021 
  * @date   : OCT 08 2020
  * @author : Julian Singkham
  * @assist : Daniel Kaehn (resolved bug in grabbing value from file to matrix)
  ***********************************************************************************
  * @attention
  *
  * This takes in the names of 2 files containing a matrix and adds them together
  * in a single process
  *
  ***********************************************************************************
**/

#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char* argv[]){
    if(argv[3] != NULL){
        printf("Error too many arguements, 2 are needed");
        return 1;
    }
    else if (argv[2] == NULL || argv[1] == NULL){
        printf("Error not enough arguements, 2 are needed.");
        return 1;
    }

    int row_matA, col_matA; 
    int row_matB, col_matB; 
    struct timespec time, end_time;

    //Check if the file is real/can be opened
    FILE *fileA = fopen(argv[1], "r"); 
    if(fileA == NULL){
        perror("File does not exist or can not be opened");
        fclose(fileA);
        return 1;
    }
    FILE *fileB = fopen(argv[2], "r"); 
    if(fileB == NULL){
        perror("File does not exist or can not be opened");
        fclose(fileA);
        fclose(fileB);
        return 1;
    }

    //The first two values of the file are the row and col size.
    fscanf(fileA, "%d", &row_matA);
    fscanf(fileA, "%d", &col_matA);
    fscanf(fileB, "%d", &row_matB);
    fscanf(fileB, "%d", &col_matB);
    
    //Check if both matrices are the same size
    if((row_matA != row_matB) || (col_matA != col_matB)){
        printf("The given matrices are not the same size %s %dx%d vs %s %dx%d", argv[1], row_matA, col_matA, argv[2], row_matB, col_matB);
        fclose(fileA);
        fclose(fileB);
        return 1;
    }

    int* matrixA = malloc(row_matA * col_matA * sizeof(int));
    int* matrixB = malloc(row_matB * col_matB * sizeof(int));
    int* output = malloc(row_matA * col_matA * sizeof(int));

    //Fill the values for matrix A and B
    for(int i = 0; i < row_matA; i++){
        for(int j = 0; j < col_matA; j++){
            fscanf(fileA, "%d", &matrixA[(i*col_matA)+j]);
            fscanf(fileB, "%d", &matrixB[(i*col_matB)+j]);
        }
    }

    clock_gettime(CLOCK_REALTIME, &time); //Start timer

    //Add up the elements in matrix A and B
    for(int i = 0; i < row_matA; i++){
        for(int j = 0; j < col_matA; j++){
            output[(i*col_matA)+j] = matrixA[(i*col_matA)+j] + matrixB[(i*col_matA)+j];
        }
    }

    clock_gettime(CLOCK_REALTIME, &end_time); //End timer

    long total_time = ((end_time.tv_sec*1000000000) + end_time.tv_nsec) - ((time.tv_sec*1000000000) + time.tv_nsec);
    printf("Total time to compute: %ld ms\n", total_time);
    
    //Print the output matrix
    for(int i = 0; i < row_matA; i++){
        for(int j = 0; j < col_matA; j++){
            
            printf("%d ", output[(i*col_matA)+j]);
        }
        printf("\n");
    }

    free(matrixA);
    free(matrixB);
    free(output);
    return 0;
}