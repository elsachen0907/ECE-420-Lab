/*
Generate a non-singular matrix and a vector and save the output. (the values are in precision double)

-----
Compiling:
    > gcc datagen.c Lab3IO.c -o datagen

-----
Synopsis:
    datagen [-sbpo]

-----
Options:
    -s    specify the size of the matrices (default 100) 
    -b    upper bound of the absolute value of the most elements (default 100)
    -p    print the result
    -o    specify the output path (default "./data_input") 

-----
Outputs:
    Output file:
    data_input

-----
Error returns:
    -1    unexpected options
    -2   fail to open file 

-----
Example:
    >datagen 
    generate 100 by 100 non singular double matrix and a vector b with most elements between -100 and 100 in "./data_input"
   
    >datagen -s 1000 -b 10 -p
    generate 1000 by 1000 non singular double matrix and a vector b with most elements between -10 and 10 in "./data_input", and print the result on the screen.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "Lab3IO.h"

#define DECIMAL 100.0

int MatMul(int, double**, double**, double**);
int MatGen(int, double**, double);
int GenPerm(int, double**);

int main (int argc, char* argv[]){   
    int i, j, option;
    int size = 100;
    int b_print = 0;
    int range = 100;
    double **A, **T, **S;
    double *b;
    double temp;
    char* OUTPATH = "data_input";
    FILE* fp;

    srand(time(NULL));

    while ((option = getopt(argc, argv, "s:b:po:")) != -1)
        switch(option){
            case 's': size = strtol(optarg, NULL, 10); break;
            case 'b': range = strtol(optarg, NULL, 10);break;
            case 'p': b_print = 1; break;
            case 'o': OUTPATH = optarg; break;
            case '?': printf("Unexpected Options. \n"); return -1;
        }
    
    /*Generate the data*/
    A = CreateMat(size, size);
    T = CreateMat(size, size);
    S = CreateMat(size, size);
    b = malloc(size * sizeof(double));
    for (i = 0; i < size; ++i)
        for (j = 0; j < size; ++j){
            A[i][j] = 0;
            T[i][j] = 0;
        }
    MatGen(size, T, (double)range);
    GenPerm(size, A);
    MatMul(size, T, A, S);
    for (i = 0; i < size; ++i){
        temp = (double)(random() % (int)(range * DECIMAL)) / DECIMAL;
        if (random() % 2)
            temp *= -1;
        b[i] = temp;
    }
    /*Output the data*/
    if ((fp = fopen(OUTPATH,"w")) == NULL){
        printf("Fail to open a file to save the data. \n");
        return -2;
    }
    fprintf(fp, "%d\n\n", size);
    for (i = 0; i < size; ++i){
        for (j = 0; j < size; ++j)
            fprintf(fp, "%lf\t", S[i][j]);
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
    for (i = 0; i < size; ++i)
        fprintf(fp, "%lf\n", b[i]);
    fclose(fp);
    /*Print the result if neccesary*/
    if (b_print){
        printf("The problem size is %d.\n", size);
        printf("============\n The A is \n============\n");
        PrintMat(S, size, size);
        printf("============\n The b is \n============\n");
        PrintVec(b, size);
    }
    DestroyMat(A, size);
    DestroyMat(T, size);
    DestroyMat(S, size);
    free(b);
    return 0;
}

int MatGen(int n, double** S, double range){
    int i,j;
    double temp;
    /*Generate a random matrix*/
    for (i = 0; i < n; ++i){
        for (j = 0; j < n; ++j){
            temp = (double)(random() % (int)(DECIMAL * range)) / DECIMAL;
            if (random() % 2)
                temp *= -1;
            S[i][j] = temp;
        }
    }
    /*Make it row dominant*/
    for (i = 0; i < n; ++i){
        temp = 0;
        for (j = 0; j < n; ++j)
            if (S[i][i] * S[i][j] > 0)
                temp += S[i][j];
            else
                temp -= S[i][j];
        S[i][i] += temp / n * 10;
    }
    return 0;
}

int MatMul(int n, double** A, double** T, double** S){
    int i, j, k;
    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j){
            S[i][j]=0;
            for (k = 0; k < n; ++k)
                S[i][j] += A[i][k] * T[k][j];
        }
    return 0;
}

int GenPerm(int n, double** A){
    int* flag;
    int i, j, k;
    int remain;
    int index;
    remain = n;
    flag = malloc(n * sizeof(int));
    for (i = 0; i < n; ++i)
        flag[i] = 0;

    for (i = 0; i < n; ++i){
        index = random() % remain + 1;
        j = 0; k = -1;
        while (j < index){
            ++k;
            if (flag[k] == 0)
                ++j;
        }
        A[i][k] = 1.0;
        flag[k] = 1; 
        --remain;
    }
    free(flag);
    return 0;
}