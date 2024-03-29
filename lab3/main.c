#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>

#include "Lab3IO.h"
#include "timer.h"

void gauss(double** G, int n)
{
    double* temp = CreateVec(n);

    int row = 0;
    double max = 0;

    #pragma omp parallel shared(G, n, max, row, temp)
    {
        int k;
        for (k = 0; k < n - 1; k++) {
            // double lmax = 0;
            // int lrow = k;
            int p;
            #pragma omp for
            for (p = k; p < n; p++) {
                double upk = fabs(G[p][k]);

                // if (upk > lmax) {
                //     lmax = upk;
                //     lrow = k;
                // }s
                if (upk > max) {
                    #pragma omp critical
                    {
                        if (upk > max) {
                            max = upk;
                            row = k;
                        }
                    }
                }
            }


            #pragma omp single
            {
                // Cleanup for next iteration
                max = 0;
                row = k + 1;

                double* swap = G[row];
                G[row] = G[k];
                G[k] = swap;
            }
            
            int i;
            #pragma omp for
            for (i = k + 1; i < n; i++) {
                temp[i] = G[i][k] / G[k][k];
            }

            // int y;
            int j;
            #pragma omp for collapse(2)
            for (i = k + 1; i < n; i++) {
                for (j = k; j < n + 1; j++) {
                    G[i][j] = G[i][j] - temp[i] * G[k][j];
                }
            }
        }
    }
}


void jordan(double** U, int n)
{
    int k, i;
    // #pragma omp parallel for private(k)
    for (k = n -1; k >= 0; k--){
        // int i;
        // #pragma omp parallel for
        for (i = n-1 ; i >= 0; i--){
            if (i != k){
                U[i][n]= U[i][n] - U[i][k] / U[k][k] * U[k][n];
                U[i][k] = 0;
            }
            
        }
    }
    // return 

}


  int main(int argc, char* argv[])
{
    int size;
    double** G;
    double t1;
    double t2;
    // double* x;

    // Read command-line arguments
    if (argc != 2){
        printf("Usage : main <numThreads>\n");
        return(1);
    }
    
    int numThreads = strtol(argv[1], NULL, 10);
    omp_set_num_threads(numThreads);
    printf("Thread count received: %s\n", argv[1]);

    // Load data
    if (Lab3LoadInput(&G, &size)==1){
        printf("Error in loading\n");
        return(1);
    }
    
    // x = malloc(size * sizeof(double));
    // x = CreateVec(size);

    GET_TIME(t1);
    gauss(G, size);
    jordan(G, size);
    GET_TIME(t2);

    PrintMat(G, size, size + 1);
    fprintf(stderr, "%f\n", t2 - t1);

    Lab3SaveOutput(*G, size, t2-t1);
    // fprintf(G);

    return 0;
}
