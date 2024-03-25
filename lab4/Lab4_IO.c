#define LAB4_EXTEND

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lab4_IO.h"

int Lab4_saveoutput(double *R, int nodecount, double Time){
/*
    Save the data to file for Lab 4 

    -----
    Input:
    int *R         pointer to the result array 
    int nodecount  number of nodes 
    double Time    measured calculation time
    
    -----
    Output:
    data_output the stored data

    -----
    Example:
    lab4_saveoutput(R, nodecount, Time);
*/    
    FILE* op;
    int i;

    if ((op = fopen("data_output","w")) == NULL) {
        printf("Error opening the input file.\n");
        return 1;
    }
    fprintf(op, "%d\n%f\n", nodecount, Time);
    for (i = 0; i < nodecount; ++i )
        fprintf(op, "%e\n", R[i]);
    fclose(op);
    return 0;
}

#ifdef LAB4_EXTEND

int node_init(struct node **nodehead, int start, int end){
    FILE *ip;
    int i, nodecount;
    int src, dst;
    int nodeID, num_in, num_out;
    int *index; //keep track of the outlink list storing position
    int num_nodes = end - start;

    // deal with the meta data and allocate space
    (*nodehead) = malloc(num_nodes * sizeof(struct node));
    if ((ip = fopen("data_input_meta","r")) == NULL) {
        printf("Error opening the data_input_meta file.\n");
        return -1;
    }
    fscanf(ip, "%d\n", &nodecount);
    for (i=0; i<start; ++i){ // allocate to the right place
        fscanf(ip, "%d\t%d\t%d\n", &nodeID, &num_in, &num_out);
    }
    for ( i = start; i < end && i < nodecount; ++i){ // handlling the dumping case
        fscanf(ip, "%d\t%d\t%d\n", &nodeID, &num_in, &num_out);
        if (nodeID != i){
            printf("Error loading meta data, node id inconsistent!\n");
            return -2;
        }
        (*nodehead)[i-start].num_in_links = num_in;
        (*nodehead)[i-start].num_out_links = num_out;
        (*nodehead)[i-start].inlinks = malloc(num_in * sizeof(int));
    }
    fclose(ip);
    if (i < end){// initial the dampling nodes as well
        for (;i<end; ++i){
            (*nodehead)[i-start].num_in_links = 0;
            (*nodehead)[i-start].num_out_links = nodecount;
            (*nodehead)[i-start].inlinks = malloc(sizeof(int));
        }
    }
    // Load the link informations
    if ((ip = fopen("data_input_link","r")) == NULL) {
        printf("Error opening the data_input_link file.\n");
        return -3;
    }
    index = malloc(num_nodes * sizeof(int));
    for(i=0; i<num_nodes; ++i){
        index[i] = 0;
    }
    while(!feof(ip)){
        fscanf(ip, "%d\t%d\n", &src, &dst);
        if (dst >= start && dst < end)
            (*nodehead)[dst - start].inlinks[index[dst - start]++] = src;
    }
    free(index);
    fclose(ip);
    return 0;
}

int node_destroy(struct node *nodehead, int num_nodes){
    int i;
    for (i = 0; i < num_nodes; ++i){
        free(nodehead[i].inlinks);
    }
    free(nodehead);
    return 0;
}

double rel_error(double *r, double *t, int size){
    int i;
    double norm_diff = 0, norm_vec = 0;
    for (i = 0; i < size; ++i){
        norm_diff += (r[i] - t[i]) * (r[i] - t[i]);
        norm_vec += t[i] * t[i];
    }
    return sqrt(norm_diff/norm_vec);
}

int vec_cp(double *r, double *t, int size){
    int i;
    for (i = 0; i < size; ++i) t[i] = r[i];
    return 0;
}

#endif // LAB4_EXTEND
