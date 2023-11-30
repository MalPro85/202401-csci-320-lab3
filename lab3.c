#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"
#include <string.h>

extern int **sudoku_board;
int *worker_validation;

int **read_board_from_file(char *filename)
{
    FILE *fp = fopen(filename, "r");
    int **board = (int **)malloc(sizeof(int *) * ROW_SIZE);
    for (int row = 0; row < ROW_SIZE; row++)
    {
        board[row] = (int *)malloc(sizeof(int) * COL_SIZE);
    }

    if (fp == NULL)
    {
        printf("Error: could not open file %s", filename);
        return NULL;
    }

    for (int i = 0; i < ROW_SIZE; i++)
    {
        for (int j = 0; j < COL_SIZE; j++)
        {
            fscanf(fp, "%d,", &board[i][j]);
        }
    }

    fclose(fp);

    worker_validation = (int *)calloc(NUM_OF_THREADS, sizeof(int));

    return board;
}

void *check_row(void *params) {

    int *check_array = (int *)calloc(ROW_SIZE, sizeof(int));

    param_struct *param = (param_struct*)(params);
    int start = (*param).starting_col, end = (*param).ending_col, curr_row = (*param).starting_row;
    int invalid = 1;

    for (int i = start; i < end; i++)
    {
        if (exists(sudoku_board[curr_row][i], check_array)) {
            invalid = 0;
            break;
        }
        else {
            check_array[i] = sudoku_board[curr_row][i];
        }
    }

    worker_validation[(*param).id] = invalid;

    free(check_array);
}

void *check_col(void *params) {

    int *check_array = (int *)calloc(COL_SIZE, sizeof(int));

    param_struct *param = (param_struct*)(params);
    int start = (*param).starting_row, end = (*param).ending_row, curr_col = (*param).starting_col;
    int invalid = 1;

    // printf("%d\n", curr_col);

    for (int i = start; i < end; i++)
    {
        if (exists(sudoku_board[i][curr_col], check_array))
        {
            invalid = 0;
            break;
        }
        else
        {
            check_array[i] = sudoku_board[i][curr_col];
        }
    }

    worker_validation[(*param).id] = invalid;

    free(check_array);
}

int exists(int element, int *arr) {

    //examine this

    int found = 0;

    // printf("element: %d\n", element);
    // printf("in exists\n");

    for (int i = 0; i < ROW_SIZE; i++) {
        // printf("%d\n", arr[i]);
        if (arr[i] == element)
        {

            found = 1;
            break;
        }
    }

    return found;
}

int is_board_valid()
{
    pthread_t *tid; /* the thread identifiers */
    pthread_attr_t attr;
    param_struct *parameters;

    tid = (pthread_t *)malloc(sizeof(pthread_t) * NUM_OF_THREADS);
    parameters = (param_struct *)malloc(sizeof(param_struct) * NUM_OF_THREADS);

    for (int i = 0; i < COL_SIZE; i++) {
        parameters[i].id = i;
        parameters[i].starting_row = i;
        parameters[i].starting_col = 0;
        parameters[i].ending_row = i;
        parameters[i].ending_col = 9;
    }

    for (int i = COL_SIZE; i < (COL_SIZE * 2); i++) {
        parameters[i].id = i;
        parameters[i].starting_row = 0;
        parameters[i].starting_col = i - 9;
        parameters[i].ending_row = 9;
        parameters[i].ending_col = i - 9;
    }

    for (int i = 0; i < COL_SIZE; i++)
    {
        pthread_create(&(tid[i]), NULL, check_row, &(parameters[i]));
        pthread_join(tid[i], NULL);
    }

    for (int i = COL_SIZE; i < (COL_SIZE * 2); i++) {
        pthread_create(&(tid[i]), NULL, check_col, &(parameters[i]));
        pthread_join(tid[i], NULL);
    }

    // TODO: validate borad and update return value

    for (int i = 0; i < COL_SIZE; i++) {
        printf("worker_validation[%d]: %d\n", i, worker_validation[i]);
    }

    for (int i = COL_SIZE; i < (COL_SIZE * 2); i++) {
        printf("worker_validation[%d]: %d\n", i, worker_validation[i]);
    }


    free(tid);
    free(parameters);
    free(worker_validation);

    return 0;
}



