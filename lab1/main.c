#include <math.h>
#include <pthread.h>
#include "lab1_IO.h"
#include "timer.h"
#include <stdlib.h>

struct mult_job {
    int ***A;
    int ***B;
    int ***C;
    int x;
    int y;
    int cell_per_block;
    int n;
};

void compute_cell(int*** A, int*** B, int* c, int i, int j, int n) {
    *c = 0;
    for (int r = 0; r < n; r++) {
        *c += (*A)[i][r] * (*B)[r][j];
    }
}

void compute_block(void* param) {
    struct mult_job* job = (struct mult_job*)(param);
    int cell_per_block = job->cell_per_block;
    for (int i = 0; i < cell_per_block; i++) {
        for (int j = 0; j < cell_per_block; j++) {
            int c_i = job->x * cell_per_block + i;
            int c_j = job->y * cell_per_block + j;
            int* c = &((*job->C)[c_i][c_j]);
            compute_cell(job->A, job->B, c, c_i, c_j, job->n);
        }
    }
}

void run_serial_computation() {
    int n;
    int** A;
    int** B;
    Lab1_loadinput(&A, &B, &n);

    int** C;
    C = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
      C[i] = malloc(n * sizeof(int));
    }

    double start_time;
    GET_TIME(start_time);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            compute_cell(&A, &B, &C[i][j], i, j, n);
        }
    }

    double end_time;
    GET_TIME(end_time);

    Lab1_saveoutput(C, &n, end_time - start_time);
}

void run_parallel_computation(int p) {
    int p_sqrt = (int)(sqrt((double)p));

    int n;
    int** A;
    int** B;

    // Load matrices
    Lab1_loadinput(&A, &B, &n);

    // Allocate C (check if this is faster than lab1_IO version)
    int** C;
    C = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
      C[i] = malloc(n * sizeof(int));
    }

    pthread_t* threads[p];
    struct mult_job jobs[p];

    double start_time;
    GET_TIME(start_time);

    // Consider possible transpose (might make it faster - might not, can optionally add)
    for (int x = 0; x < p_sqrt; x++) {
        for (int y = 0; y < p_sqrt; y++) {
            struct mult_job *job = &jobs[x*p_sqrt+y];
            job->A = &A;
            job->B = &B;
            job->C = &C;
            job->x = x;
            job->y = y;
            job->cell_per_block = n/p_sqrt;
            job->n = n;
            pthread_create(&threads[x*p_sqrt+y], NULL, *compute_block, (void*)(job));
        }
    }

    for (int i = 0; i < p; i++) {
        pthread_join(threads[i], NULL);
    }

    double end_time;
    GET_TIME(end_time);

    Lab1_saveoutput(C, &n, end_time - start_time);
}

int main(int narg, char** agrv) {
    int p = atoi(agrv[1]);

    if (p == -1) {
        run_serial_computation();
    } else {
        run_parallel_computation(p);
    }

    return 0;
}