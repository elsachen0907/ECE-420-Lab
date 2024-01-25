/* File:  
 *    arrayRW.c
 *
 * Purpose:
 *    Illustrate multithreaded reads and writes to a shared array
 *    Note that it is requred to use the "getContent" and "SaveContent" function 
 #    in "common.h" to write and read the content
 *
 * Input:
 *    none
 * Output:
 *    message from each thread
 *
 * Usage:    ./arrayRW <thread_count>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <string.h>
#include "timer.h"
#include "common.h"
#define NUM_STR 1024
#define STR_LEN 1000

int thread_count;  
char **theArray; 
unsigned int* seed;
pthread_mutex_t mutex;

void *Operate(void* rank);  /* Thread function */



/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    long       thread;  /* Use long in case of a 64-bit system */
    pthread_t* thread_handles; 
    int i;
    double start, finish, elapsed;  

    /* Get number of threads from command line */
    thread_count = strtol(argv[1], NULL, 10);  
    
    /* Intializes random number generators */
    seed = malloc(thread_count*sizeof(unsigned int));
    for (i = 0; i < thread_count; i++)
        seed[i] = i;
    
    /* Create the memory and fill in the initial values for theArray */
    theArray = (char**) malloc(NUM_STR * sizeof(char*));
    for (i = 0; i < NUM_STR; i ++){
        theArray[i] = (char*) malloc(STR_LEN * sizeof(char));
        sprintf(theArray[i], "theArray[%d]: initial value", i);
        printf("%s\n\n", theArray[i]);
    }
   
    thread_handles = malloc (thread_count*sizeof(pthread_t)); 
    pthread_mutex_init(&mutex, NULL);
    
    GET_TIME(start); 
    for (thread = 0; thread < thread_count; thread++)  
        pthread_create(&thread_handles[thread], NULL, Operate, (void*) thread);  

    for (thread = 0; thread < thread_count; thread++) 
        pthread_join(thread_handles[thread], NULL); 
    GET_TIME(finish);
    elapsed = finish - start;   
    printf("The elapsed time is %e seconds\n", elapsed);
   
    pthread_mutex_destroy(&mutex);
    free(thread_handles);
    free(seed);
    for (i=0; i<NUM_STR; ++i){
        free(theArray[i]);
    }
    free(theArray);
    return 0;
}  /* main */


/*-------------------------------------------------------------------*/
void *Operate(void* rank) {
    long my_rank = (long) rank;
    char buffer[STR_LEN]; // buffer to read and write
    // Find a random position in theArray for read or write
    int pos = rand_r(&seed[my_rank]) % NUM_STR;
    int randNum = rand_r(&seed[my_rank]) % 10;  // write with 10% probability
    sprintf(buffer, "theArray[%d] modified by thread %ld", pos, my_rank); // prepare the message

    pthread_mutex_lock(&mutex); 
    // write the content
    if (randNum >= 9){ // 10% are write operations, others are reads
        setContent(buffer, pos, (char**)theArray); // defined in "common.h"
    }
    printf("Thread %ld: randNum = %d\n", my_rank, randNum);
    // read the content and display
    getContent(buffer, pos, (char**)theArray); // defined in "common.h"
    printf("%s\n\n", buffer); // display the result 
    pthread_mutex_unlock(&mutex);
    return NULL;
}