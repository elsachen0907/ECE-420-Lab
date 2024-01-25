/*
    Client that send read write request to test the server speed 
*/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include <errno.h>
#include "common.h"

unsigned int* seed_;
struct sockaddr_in sock_var_;
int NUM_STR_;
int PushPullMessage(char* str_msg, char* str_rcv); // function to send and recieve message
/*-------------------------------------------------------------------*/
void *Request(void* rank) {
    long my_rank = (long) rank;  // Use long in case of 64-bit system 
    char str_msg[COM_BUFF_SIZE], str_rcv[COM_BUFF_SIZE];
    if (COM_NUM_REQUEST % COM_CLIENT_THREAD_COUNT != 0){
        printf("COM_NUM_REQUEST is not divisible by COM_CLIENT_THREAD_COUNT\n");
        exit(-1);
    }
    int num_request = COM_NUM_REQUEST / COM_CLIENT_THREAD_COUNT;
    int i;
    // main loop for reqeust
    for (i=0; i<num_request; ++i){
        // Prepare the message
        int pos = rand_r(&seed_[my_rank]) % NUM_STR_; // generate a dest position
        int is_read = rand_r(&seed_[my_rank]) % 100 < 70 ? 1 : 0;    // write with 30% probability
        sprintf(str_msg, "%d-%d-String[%d] has been modified by thread %ld", pos, is_read, pos, my_rank); // encode the message and control signal into a single string
        // Connect and request
        PushPullMessage(str_msg, str_rcv);
        // Display
        if (1 == COM_IS_VERBOSE){
            printf("Client %ld sent: %s\n\t receive: %s\n", my_rank, str_msg, str_rcv);
        }
    }
    return NULL;
} // end Request

int main(int argc, char* argv[]){
    long       thread;  /* Use long in case of a 64-bit system */
    pthread_t* thread_handles; 
    int i;
    /* Command line parameter count should be 4 */
    if (argc != 4){ 
        fprintf(stderr, "usage: %s <Size of theArray_ on server> <server ip> <server port>\n", argv[0]);
        exit(0);
    }
    NUM_STR_ = strtol(argv[1], NULL, 10);
    char *server_ip = argv[2];
    int server_port = strtol(argv[3], NULL, 10);

    /* Intializes random number generators */
    seed_ = malloc(COM_CLIENT_THREAD_COUNT*sizeof(unsigned int));
    for (i = 0; i < COM_CLIENT_THREAD_COUNT; i++)
        seed_[i] = i;
    thread_handles = malloc(COM_CLIENT_THREAD_COUNT*sizeof(pthread_t)); 
    /* Initialize socket address and port*/
    sock_var_.sin_addr.s_addr=inet_addr(server_ip);
    sock_var_.sin_port=server_port;
    sock_var_.sin_family=AF_INET;
    /* Create threads */
    for (thread = 0; thread < COM_CLIENT_THREAD_COUNT; thread++)  
            pthread_create(&thread_handles[thread], NULL,
                Request, (void*) thread);  
    /* Finalize threads */
    for (thread = 0; thread < COM_CLIENT_THREAD_COUNT; thread++) 
            pthread_join(thread_handles[thread], NULL); 
    free(thread_handles);
    return 0;
} // end main

// Handling the connect and transmit for client
int PushPullMessage(char* str_msg, char* str_rcv){
    int clientFileDiscriptor=socket(AF_INET,SOCK_STREAM,0);
    if(clientFileDiscriptor < 0){
        int errsv = errno;
        printf("socket() failed with errno = %d. \n", errsv);
        exit(errsv);
    }
    if(0 == connect(clientFileDiscriptor,(struct sockaddr*)&sock_var_,sizeof(sock_var_))){
        // Write
        if(0 > write(clientFileDiscriptor, str_msg, COM_BUFF_SIZE)){
            int errsv = errno;
            printf("write() failed with errno = %d. \n", errsv);
            exit(errsv);
        }
        // Read
        if(0 > read(clientFileDiscriptor,str_rcv,COM_BUFF_SIZE)){
            int errsv = errno;
            printf("read() failed with errno = %d. \n", errsv);
            exit(errsv);
        }
    }else{
        int errsv = errno;
        printf("connect() failed with errno = %d. \n", errsv);
        exit(errsv);
    }
    if(0 != close(clientFileDiscriptor)){
        int errsv = errno;
        printf("close() failed with errno = %d. \n", errsv);
        exit(errsv);
    }
    return 0;
}