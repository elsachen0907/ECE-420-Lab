#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

int main(int argc, char* argv[])
{
    struct sockaddr_in sock_var;
    int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    char str_clnt[20],str_ser[20];

    sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
    sock_var.sin_port=3000;
    sock_var.sin_family=AF_INET;

    if(connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
        printf("Connected to server %d\n",clientFileDescriptor);
        printf("Enter Srting to send\n");
        scanf("%s",str_clnt);
        write(clientFileDescriptor,str_clnt,20);
        read(clientFileDescriptor,str_ser,20);
        printf("String from Server: %s\n",str_ser);
        close(clientFileDescriptor);
    }
    else{
        printf("socket creation failed");
    }
    return 0;
}