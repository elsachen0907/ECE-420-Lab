#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

#include "common.h"

/**
 * TODO:
 * 0. Read input √
 * 1. Handle multithreaded client (default=100) √
 * 2. Create array of n strings √
 * 3. Parse request and call read or write function
 * 4. Implement read and write function
 * 5. Implement a better parallelization than mutex
*/

int theLen;
char** theArray;
pthread_rwlock_t* theLocks;

pthread_mutex_t nextClientMutex;
pthread_cond_t handlerIsIdleCond;
pthread_cond_t incomingClientCond;

int incomingClient = NULL;
int idleHandlers = 0;

void* handle(void*)
{
    int clientSocket;
    char str[COM_BUFF_SIZE];
    ClientRequest req;

    while (1) {
        pthread_mutex_lock(&nextClientMutex);
        while (incomingClient == NULL) {
            idleHandlers++;
            pthread_cond_signal(&handlerIsIdleCond);
            pthread_cond_wait(&incomingClientCond, &nextClientMutex);
            idleHandlers--;
        }

        clientSocket = incomingClient;
        incomingClient = NULL;

        pthread_mutex_unlock(&nextClientMutex);

        read(clientSocket, str, COM_BUFF_SIZE);
        ParseMsg(&str, &req);

        char out[COM_BUFF_SIZE];
        if (req.is_read > 0) {
            pthread_rwlock_rdlock(&(theLocks[req.pos]));
        } else {
            pthread_rwlock_wrlock(&(theLocks[req.pos]));
        }

        getContent(out, req.pos, theArray);

        if (req.is_read == 0) {
            setContent(req.msg, req.pos, theArray);
        }

        pthread_rwlock_unlock(&(theLocks[req.pos]));

        write(clientSocket, out, COM_BUFF_SIZE);
        close(clientSocket);

    }
    return NULL;
}

int main(int argc, char* argv[])
{
    // Read command-line arguments
    theLen = strtol(argv[1], NULL, 10);
    char* server_ip = argv[2];
    int server_port = strtol(argv[3], NULL, 10);

    // Initialize theArray and theLocks
    theArray = (char**)malloc(theLen * sizeof(char*));
    theLocks = (pthread_rwlock_t*)malloc(theLen * sizeof(pthread_rwlock_t));
    for (int i = 0; i < theLen; i++) {
        theArray[i] = (char*)malloc(COM_BUFF_SIZE * sizeof(char));
        pthread_rwlock_init(&(theLocks[i]), NULL);
    }

    // Initialize the threads
    pthread_t* t = malloc(COM_NUM_REQUEST*sizeof(pthread_t));
    for (int i = 0; i < COM_NUM_REQUEST; i++) {
        pthread_create(&t[i], NULL, handle, NULL);
    }

    // Initialize socket info
    struct sockaddr_in sock_var;
    int hostSocket=socket(AF_INET,SOCK_STREAM,0);

    // Setup server info
    sock_var.sin_addr.s_addr=inet_addr(server_ip);
    sock_var.sin_port=server_port;
    sock_var.sin_family=AF_INET;
    if (
        bind(
            hostSocket,
            (struct sockaddr*) &sock_var,
            sizeof(sock_var)
        ) < 0
    ) {
        printf("Failed to bind socket - exiting\n");
    }

    listen(hostSocket,2000);
    while (1) {
        int next = accept(hostSocket, NULL, NULL);

        pthread_mutex_lock(&nextClientMutex);
        while (idleHandlers <= 0) {
            pthread_cond_wait(&handlerIsIdleCond, &nextClientMutex);
        }

        incomingClient = next;
        pthread_cond_signal(&incomingClientCond);
        pthread_mutex_unlock(&nextClientMutex);
    }
    close(hostSocket);
    return 0;
}