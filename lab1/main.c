#include <math.h>
#include <pthread.h>
#include "lab1_IO.h"
#include "timer.h"

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
            int* c = &((*job->C)[job->x * cell_per_block + i][job->y * cell_per_block + j]);
            compute_cell(job->A, job->B, c, job->x * cell_per_block + i, job->y * cell_per_block + j, job->n);
        }
    }
}

int main(int narg, char** agrv) {
    int p = atoi(agrv[1]);
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

    return 0;
}