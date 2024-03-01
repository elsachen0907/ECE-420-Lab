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
        for (int k = 0; k < n - 1; k++) {
            // double lmax = 0;
            // int lrow = k;

            #pragma omp for
            for (int p = k; p < n; p++) {
                double upk = fabs(G[p][k]);

                // if (upk > lmax) {
                //     lmax = upk;
                //     lrow = k;
                // }
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

            // if (lmax > max) {
            //     #pragma omp critical
            //     {
            //         if (lmax > max) {
            //             max = lmax;
            //             row = lrow;
            //         }
            //     }
            // }
            // #pragma omp barrier

            #pragma omp single
            {
                // Cleanup for next iteration
                max = 0;
                row = k + 1;

                double* swap = G[row];
                G[row] = G[k];
                G[k] = swap;
            }

            #pragma omp for
            for (int i = k + 1; i < n; i++) {
                temp[i] = G[i][k] / G[k][k];
            }

            #pragma omp for collapse(2)
            for (int i = k + 1; i < n; i++) {
                for (int j = k; j < n + 1; j++) {
                    G[i][j] = G[i][j] - temp[i] * G[k][j];
                }
            }
        }
    }
}

double** gauss_nomp(double** G, int n)
{
    double** U = CreateMat(n, n + 1);
    for (int i = 0; i < n; i++) {
        U[i] = memcpy(U[i], G[i], (n+1) * sizeof(double));
    }

    for (int k = 0; k < n - 1; k++) {
        double max = 0;
        int row = k;
        for (int p = k; p < n; p++) {
            if (fabs(U[p][k]) > max) {
                max = fabs(U[p][k]);
                row = p;
            }
        }

        double* swap = U[row];
        U[row] = U[k];
        U[k] = swap;

        for (int i = k + 1; i < n; i++) {
            double temp = U[i][k] / U[k][k];
            for (int j = k; j < n + 1; j++) {
                U[i][j] = U[i][j] - temp * U[k][j];
            }
        }
    }

    return U;
}

void jordan()
{

}

int main(int argc, char* argv[])
{
    int size;
    double** G;
    double t1;
    double t2;

    // Read command-line arguments
    int numThreads = strtol(argv[1], NULL, 10);
    omp_set_num_threads(numThreads);

    // Load data
    Lab3LoadInput(&G, &size);

    GET_TIME(t1);
    gauss(G, size);
    GET_TIME(t2);

    PrintMat(G, size, size + 1);
    fprintf(stderr, "%f\n", t2 - t1);

    return 0;
}