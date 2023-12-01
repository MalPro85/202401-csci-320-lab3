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

void *check_square(void *params) {
    
    int *check_array = (int *)calloc(COL_SIZE, sizeof(int));

    param_struct *param = (param_struct *)(params);
    int top_row = (*param).starting_row, bottom_row = (*param).ending_row, left_col = (*param).starting_col, right_col = (*param).ending_col;
    int invalid = 1;
    int idx = 0;

    for (int i = top_row; i < bottom_row; i++) {
        
        for (int j = left_col; j < right_col; j++) {
            if (exists(sudoku_board[i][j], check_array))
            {
                invalid = 0;
                break;
            }
            else {
                check_array[idx] = sudoku_board[i][j];
                idx++;
            }
        }

        if (invalid == 0)
            break;
    }

    worker_validation[(*param).id] = invalid;
    free(check_array);
}

int exists(int element, int *arr) {

    int found = 0;

    for (int i = 0; i < ROW_SIZE; i++) {
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

    int idx = 18;

    for (int i = 0; i < 9; i+=3) {
        for (int j = 0; j < 9; j+=3) {
            parameters[idx].id = idx;
            parameters[idx].starting_row = i;
            parameters[idx].ending_row = i + 3;
            parameters[idx].starting_col = j;
            parameters[idx].ending_col = j + 3;
            idx++;
        }
    }

    pthread_create(&(tid[18]), NULL, check_square, &parameters[18]);
    pthread_join(tid[18], NULL);

    for (int i = 0; i < COL_SIZE; i++)
    {
        pthread_create(&(tid[i]), NULL, check_row, &(parameters[i]));
        pthread_join(tid[i], NULL);
    }

    for (int i = COL_SIZE; i < (COL_SIZE * 2); i++) {
        pthread_create(&(tid[i]), NULL, check_col, &(parameters[i]));
        pthread_join(tid[i], NULL);
    }

    for (int i = 18; i < NUM_OF_THREADS; i++) {
        pthread_create((&tid[i]), NULL, check_square, &(parameters[i]));
        pthread_join(tid[i], NULL);
    }

    // for (int i = 0; i < COL_SIZE; i++) {
    //     printf("worker_validation[%d]: %d\n", i, worker_validation[i]);
    // }

    // for (int i = COL_SIZE; i < (COL_SIZE * 2); i++) {
    //     printf("worker_validation[%d]: %d\n", i, worker_validation[i]);
    // }

    // for (int i = 18; i < NUM_OF_THREADS; i++) {
    //     printf("worker_validation[%d]: %d\n", i, worker_validation[i]);
    // }

    int validity = 1;

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (worker_validation[i] == 0) {
            validity = 0;
            break;
        }
    }

    free(tid);
    free(parameters);
    free(worker_validation);

    return validity;
}






