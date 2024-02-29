#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "Lab3IO.h"
#include "timer.h"

int numThreads;

double** gauss(double** G, int n)
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
    double** U;

    // Read command-line arguments
    numThreads = strtol(argv[1], NULL, 10);

    // Load data 
    Lab3LoadInput(&G, &size);

    U = gauss(G, size);

    PrintMat(U, size, size + 1);

    return 0;
}