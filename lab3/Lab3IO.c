#include <stdio.h>
#include <stdlib.h>
#include "Lab3IO.h"

int Lab3LoadInput(double ***A, int *size){
/*
    Allocate memory and load the input data for Lab 3. The returned matrix is the augmented size by (size+1) matrix [A|b]

    -----
    Input:
    int ***A    pointer to the augmented matrix [A|b]
    int *size   pointer to the rows of the augmented matrix [A|b]. (Number of columns will be 1 more)

    Note: original files should be the output of the datagen.c with name "data_input" in the same folder

    -----
    Output:
    Generated matrix will be passed back to the array *A, along with the matrix size in *size 

    -----
    Example:
    An integer array pointer and a integer should be defined before calling this function:
    int **A; int size; 
    call this function as
    Lab3LoadInput(&A, &size);
*/

    FILE* ip;
    int i,j;

    if ((ip = fopen("data_input","r")) == NULL){
            printf("error opening the input data.\n");
            return 1;
    }
    fscanf(ip, "%d\n\n", size);
    (*A) = CreateMat(*size, (*size) + 1);
    for (i = 0; i < *size; ++i){
        for(j = 0; j < *size; ++j)
            fscanf(ip, "%lf\t", &(*A)[i][j]);
        fscanf(ip, "\n");
    }
    fscanf(ip, "\n");
    for (i = 0; i < *size; ++i)
        fscanf(ip, "%lf\n", &(*A)[i][(*size - 1) + 1]);
    fclose(ip);
    return 0;
}

int Lab3SaveOutput(double* x, int size, double Time){
/*
    Save the data to the file for Lab 3 

    -----
    Input:
    double* x     pointer to the result vector 
    int size      the vector size
    double Time   calulation time
    
    -----
    Output:
    data_output the stored data

    -----
    Example:
    Lab3SaveOutput(x, size, Time);
*/

    FILE* op;
    int i;

    if ((op = fopen("data_output","w")) == NULL){
        printf("Error opening the output file.\n");
        return 1;
    }

    fprintf(op, "%d\n", size);
    for (i = 0; i < size; ++i)
        fprintf(op, "%e\t", x[i]);
    fprintf(op, "\n%lf", Time);
    fclose(op);
    return 0;
}
 
double** CreateMat(int NumRow, int NumCol){
    /* Allocate memory for an array
    -----
    Input:
        NumRow    Number of rows
        NumCol    Number of columns
    -----
    Output:
        ** A    pointer to the new int array
    */
    int i;
    double ** A;

    A = malloc(NumRow * sizeof(double*));
    for (i = 0; i < NumRow; ++i){
        A[i] = malloc(NumCol * sizeof(double));
    }
    return A;
}
 
int DestroyMat(double** A, int NumRow){
    /* Free the memory
    -----
    Input:
        NumRow    Number of rows
    */
    int i;
    for (i = 0; i < NumRow; ++i){
        free(A[i]);
    }
    free(A);
    return 0;
}

int PrintMat(double** A, int NumRow, int NumCol){
    /* Print an array
    -----
    Input:
        A         pointer to the array
        NumRow    Number of rows
        NumCol    Number of columns
    */
    int i, j;
    for (i = 0; i < NumRow; ++i){
        for (j = 0; j < NumCol; ++j){
            printf("%f\t", A[i][j]);
        }
        printf("\n");
    }
    return 0;
}

double* CreateVec(int size){
    double *b;
    b =  malloc(size * sizeof(double));
    return b;
}

int PrintVec(double* b, int size){
    int i;
    for (i = 0; i< size; ++i){
        printf("%f\n", b[i]);
    }
    return 0;
}

int DestroyVec(double* b){
    free(b);
    return 0;
}