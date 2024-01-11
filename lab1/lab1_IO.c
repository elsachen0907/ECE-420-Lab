#include <stdio.h>
#include <stdlib.h>
#include "lab1_IO.h"

int Lab1_loadinput(int ***A, int ***B, int *n)
{
/*
    Allocate memory and load the input data for Lab 1

    -----
    Input:
    int ***A    pointer to the matrix A
    int ***B    pointer to the matrix B
    int *n      pointer to the matrix size

    original files should be the output of the matrixgen.c with name "data_input" in the same folder

    -----
    Output:
    Generated matrices will be passed back to the array *A and *B

    -----
    Example:
    Two integer array pointers and a integer should be defined before calling this function:
    int **A; int **B; int n
    call this function as
    lab1_loadinput(&A, &B, &n);
*/

    FILE* ip;
    int i,j;

    if ((ip = fopen("data_input","r")) == NULL)
    {
            printf("error opening the input data.\n");
            return 1;
    }
    fscanf(ip, "%d\n", n);

    *A = malloc(*n * sizeof(int*));
    *B = malloc(*n * sizeof(int*));

    for (i = 0; i < *n; i++)
    {
      (*A)[i] = malloc(*n * sizeof(int));
      (*B)[i] = malloc(*n * sizeof(int));
    }

    for (i = 0; i < *n; i++)
        for (j = 0; j< *n; j++)
            fscanf(ip, "%d\t", &(*A)[i][j]);
    for (i = 0; i < *n; i++)
        for (j = 0; j <* n; j++)
            fscanf(ip, "%d\t", &(*B)[i][j]);
    fclose(ip);
    return 0;
}

int Lab1_saveoutput(int **C, int *n, double Time)
{
/*
    Save the data to the file for Lab 1 

    -----
    Input:
    int **C     pointer to the result matrix 
    int *n      pointer to the matrix size
    double Time measure calulation time
    
    -----
    Output:
    data_output the stored data

    -----
    Example:
    lab1_loadinput(C, &n, Time);
*/

    FILE* op;
    int i,j;

    if ((op = fopen("data_output","w")) == NULL)
    {
        printf("Error opening the output file.\n");
        return 1;
    }

    fprintf(op, "%d\n\n", *n);
    for (i = 0; i <* n; i++)
    {
        for (j = 0; j< *n; j++)
            fprintf(op, "%d\t", C[i][j]);
        fprintf(op, "\n");
    }
    fprintf(op, "%f\n", Time);
    fclose(op);
    return 0;
}
 
