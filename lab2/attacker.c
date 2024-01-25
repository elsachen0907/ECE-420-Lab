/*
    Client that send read write request to test the consistency of read and write
*/
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>
#include<math.h>
#include<string.h>
#include "common.h"

int NUM_MSG_ = 4;
int NUM_READER_ = 1;
int NUM_WRITER_ = 7;
int NUM_THREADS_;
int NUM_ATTACKES_ = 4;

int is_valid_ = 1;
char **msg_pool;
unsigned int* seed_;
struct sockaddr_in sock_var_;
int NUM_STR_;
int attack_pos_; // array place to attack

int PushPullMessage(char* str_msg, char* str_rcv); // function to send and recieve message
void rand_str(char *dest, size_t length); //function to generate random string
int is_valid(char *str); // test whether the recived message is valid
/*-------------------------------------------------------------------*/
void *Writer(void* rank) {
    long my_rank = (long) rank;  // Use long in case of 64-bit system 
    char str_msg[COM_BUFF_SIZE], str_rcv[COM_BUFF_SIZE];
    int num_request = COM_NUM_REQUEST / NUM_THREADS_;
    int i;
    // main loop for reqeust
    for (i=0; i<num_request; ++i){
        // Prepare the message
        int pos = attack_pos_; // attack a fixed position
        int is_read = 0;
        int msg_idx = rand_r(&seed_[my_rank]) % NUM_MSG_;
        sprintf(str_msg, "%d-%d-%s", pos, is_read, msg_pool[msg_idx]); // encode the message and control signal into a single string
        // Connect and request
        PushPullMessage(str_msg, str_rcv);
        // Display
        if (1 == COM_IS_VERBOSE){
            printf("Client %ld sent: %s\n\t receive: %s\n", my_rank, str_msg, str_rcv);
        }
    }
    return NULL;
} // end Request

void *Reader(void* rank) {
    long my_rank = (long) rank;  // Use long in case of 64-bit system 
    char str_msg[COM_BUFF_SIZE], str_rcv[COM_BUFF_SIZE];
    int num_request = COM_NUM_REQUEST / NUM_THREADS_;
    int i;
    NUM_THREADS_ = NUM_READER_ + NUM_WRITER_;
    // main loop for reqeust
    for (i=0; i<num_request; ++i){
        // Prepare the message
        int pos = attack_pos_; // attack a fixed position
        int is_read = 1;
        int msg_idx = rand_r(&seed_[my_rank]) % NUM_MSG_;
        sprintf(str_msg, "%d-%d-%s", pos, is_read, msg_pool[msg_idx]); // encode the message and control signal into a single string
        // Connect and request
        PushPullMessage(str_msg, str_rcv);
        // Test whether the message is valid
        if (i>=1){ // leave some time for writers to initialize the server content
            is_valid(str_rcv);
        }// Display
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
    NUM_THREADS_ = NUM_WRITER_ + NUM_READER_;
    /* Command line parameter count should be 4 */
    if (argc != 4){ 
        fprintf(stderr, "usage: %s <Size of theArray_ on server> <server ip> <server port>\n", argv[0]);
        exit(0);
    }
    NUM_STR_ = strtol(argv[1], NULL, 10);
    char *server_ip = argv[2];
    int server_port = strtol(argv[3], NULL, 10);
    // generate the message pool to send
    msg_pool = malloc(NUM_MSG_ * sizeof(char*));
    for (i=0; i<NUM_MSG_; ++i){
        msg_pool[i] = malloc(COM_BUFF_SIZE * sizeof(char));
        rand_str(msg_pool[i], COM_BUFF_SIZE - 5 - log10l(NUM_STR_));
    }
    /* Intializes random number generators */
    seed_ = malloc(NUM_THREADS_*sizeof(unsigned int));
    for (i = 0; i < NUM_THREADS_; i++)
        seed_[i] = i;
    thread_handles = malloc(NUM_THREADS_*sizeof(pthread_t)); 
    /* Initialize socket address and port*/
    sock_var_.sin_addr.s_addr=inet_addr(server_ip);
    sock_var_.sin_port=server_port;
    sock_var_.sin_family=AF_INET;
    /* Main loop, attack multiple positions */
    for (i=0; i<NUM_ATTACKES_; ++i){
        attack_pos_ = NUM_STR_ / NUM_ATTACKES_ * i; 
        /* Create writer*/
        for (thread = 0; thread < NUM_WRITER_; thread++){
            pthread_create(&thread_handles[thread], NULL, Writer, (void*) thread);
        }
        /* Create reader */
        for (thread = NUM_WRITER_; thread < NUM_THREADS_; thread++){  
            pthread_create(&thread_handles[thread], NULL, Reader, (void*) thread);
        } 
        /* Finalize threads */
        for (thread = 0; thread < NUM_THREADS_; thread++) 
            pthread_join(thread_handles[thread], NULL); 
    }
    // Free the memory
    free(thread_handles);
    for (i=0; i<NUM_MSG_; ++i){
        free(msg_pool[i]);
    }
    free(msg_pool);
    if (1 == is_valid_){
        printf("Congratulations! The locks seems to work! \n");
        return 0;
    }else{
        printf("The content is inconsistent. Check your implementation. \n");
        return 1;
    }
} // end main

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

// generate random string
void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

// test whether the recieved message is consistent
int is_valid(char* str){
    int i;
    // printf("tested string: %s\n", str);
    for (i=0; i<NUM_MSG_; ++i){
        // printf("target: %s\n", msg_pool[i]);
        if (0 == strcmp(str, msg_pool[i])){
            return 1;
        }
    }
    is_valid_ = 0;
    return 0;
}