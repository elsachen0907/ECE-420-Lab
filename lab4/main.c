/*
    Serial Implementation of Lab 4
*/

#define LAB4_EXTEND

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lab4_IO.h"
#include "timer.h"
#include <mpi.h>

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85

int page_rank();
int main (int argc, char* argv[]){
    page_rank();
    return 0;
}


int page_rank() {
    // instantiate variables
    struct node *nodehead;
    int nodecount;
    double *r, *r_pre;
    int i, j;
    int iterationcount;
    double start, end;
    FILE *ip;
    /* INSTANTIATE MORE VARIABLES IF NECESSARY */
    // number of processes
    int comm_sz;
    int rank;
    double damp_const;
    // int local_nodecount;
    int error;
    int blocksize, elementcount;
    int mystart, myend;
    double *contribution, *r_local, *contribution_local;

    MPI_Init(NULL, NULL); 

    /* Get the number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 

    /* Get my rank among all the processes */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

    // load data 
    if ((ip = fopen("data_input_meta","r")) == NULL) {
        printf("Error opening the data_input_meta file.\n");
        MPI_Finalize();
        return 253;
    }
    
    fscanf(ip, "%d\n", &nodecount);
    fclose(ip);
    // local_nodecount = nodecount/comm_sz;

    if (node_init(&nodehead, 0, nodecount)) return 254;
    // not perfectly divisible
    blocksize = (int)(nodecount / comm_sz);
    if (blocksize * comm_sz != nodecount) {
        blocksize += 1;
    }
    elementcount = blocksize * comm_sz;

    // initialize variables
    r = malloc(elementcount * sizeof(double));
    r_pre = malloc(elementcount * sizeof(double));
    r_local = malloc(blocksize * sizeof(double));

    for (i = 0; i < nodecount; ++i)
        r[i] = 1.0 / nodecount;

    contribution = malloc(elementcount * sizeof(double));
    contribution_local = malloc(blocksize * sizeof(double));
    for (i = 0; i < nodecount; ++i)
        contribution[i] = r[i] / nodehead[i].num_out_links * DAMPING_FACTOR;
    damp_const = (1.0 - DAMPING_FACTOR) / nodecount;

    // CORE CALCULATION
    GET_TIME(start);

    do {
        if (rank == 0) {
            ++iterationcount;
            vec_cp(r, r_pre, nodecount);
        }

        mystart = rank * blocksize;
        myend = (rank + 1) * blocksize - 1;

        // update the value
        for (i = mystart; i <= myend && i < nodecount; ++i) {
            r_local[i - mystart] = 0;
            for (j = 0; j < nodehead[i].num_in_links; ++j)
                r_local[i - mystart] += contribution[nodehead[i].inlinks[j]];
            r_local[i - mystart] += damp_const;
        }

        // update and broadcast the contribution
        for (i = mystart; i <= myend && i < nodecount; ++i) {
            contribution_local[i - mystart] = r_local[i - mystart] / nodehead[i].num_out_links * DAMPING_FACTOR;
        }

        // send & recv buffer
        MPI_Gather(r_local, blocksize, MPI_DOUBLE, r, blocksize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        MPI_Allgather(contribution_local, blocksize, MPI_DOUBLE, contribution, blocksize, MPI_DOUBLE, MPI_COMM_WORLD);

        if (rank == 0) {
            error = (rel_error(r, r_pre, nodecount) >= EPSILON);
        }
        // one-to-all broadcast
        MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
    } while (error);
    GET_TIME(end);

    MPI_Finalize();

    if (rank == 0) {
        printf("Converged at %d th iteration. Time taken is %f.\n", iterationcount, end - start);
        Lab4_saveoutput(r, nodecount, end - start);

        node_destroy(nodehead, nodecount);
        free(contribution);

        free(r);
        free(r_pre);
        free(r_local);
        free(contribution_local);
    }
    return 0;
}
