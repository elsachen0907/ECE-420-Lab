#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

#include "common.h"
#include "timer.h"

int theLen;
char** theArray;
pthread_rwlock_t* theLocks;

pthread_mutex_t nextClientMutex;
pthread_cond_t handlerIsIdleCond;
pthread_cond_t incomingClientCond;

int clients[COM_NUM_REQUEST];
double times[COM_NUM_REQUEST];

void* handle(void* vid)
{
    int id = (int)vid;
    int clientSocket = clients[id];
    char str[COM_BUFF_SIZE];
    ClientRequest req;

    double start, end;

    read(clientSocket, str, COM_BUFF_SIZE);
    ParseMsg(&str, &req);
    GET_TIME(start);

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

    GET_TIME(end);
    write(clientSocket, out, COM_BUFF_SIZE);
    close(clientSocket);

    times[id] = end - start;
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

    // Initialize the mutex and conditions
    pthread_mutex_init(&nextClientMutex, NULL);
    pthread_cond_init(&handlerIsIdleCond, NULL);
    pthread_cond_init(&incomingClientCond, NULL);

    // Initialize the threads
    pthread_t* t = malloc(COM_NUM_REQUEST*sizeof(pthread_t));

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

    listen(hostSocket, 2000);
    while (1) {

        for (int i = 0; i < COM_NUM_REQUEST; i++) {
            clients[i] = accept(hostSocket, NULL, NULL);
            pthread_create(&t[i], NULL, handle, (void*)i);
        }

        for (int i = 0; i < COM_NUM_REQUEST; i++) {
            pthread_join(t[i], NULL);
        }

        saveTimes(times, COM_NUM_REQUEST);
    }
    close(hostSocket);
    return 0;
}