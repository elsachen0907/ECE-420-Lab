#include <stdio.h>

#include "Lab3IO.h"
#include "timer.h"

int numThreads;

void gauss(double** G, int n)
{
    double** U = CreateMat(n, n + 1);
    for (int i = 0; i < n; i++) {
        U[i] = memcpy(U[i], G[i], (n+1) * sizeof(double));
    }

    for (int k = 1; k < n; k++) {
        double max = 0;
        int row = k;
        for (int p = k; p < n; p++) {
            if (U[p][k] > max) {

            }
        }
    }

}

void jordan()
{

}

int main(int argc, char* argv[])
{
    int size;
    double** G;
    // Read command-line arguments
    numThreads = strtol(argv[1], NULL, 10);
    // Load data 
    Lab3LoadInput(&G, &size);


    return 0;
}