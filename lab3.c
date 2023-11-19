#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"
#include <string.h>

extern int** sudoku_board;
int* worker_validation;

int** read_board_from_file(char* filename)
{
    FILE *fp = NULL;
    int** board = NULL;
    for (int row = 0; row < ROW_SIZE; row++)
    {
    	board[row] = (int *)malloc(sizeof(int) * COL_SIZE);
    }
    
    if(fp ==NULL)
    {
    	printf("Error: could not open file %s", filename);
    	return NULL;
    }
    
    for (int i = 0; i < ROW_SIZE; i++)
    {
    	for (int j = 0; j < COL_SIZE; j++)
    	{
    		fscanf(fp, "%d,", &board[i][j];
    	}
    }
    
    fclose(fp);

    return board;
}


int is_board_valid(){
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameter;
    
    // replace this comment with your code
    
}
