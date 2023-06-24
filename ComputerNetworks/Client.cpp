#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

int main()
{
    int sockfd, conn_fd, m;
    struct sockaddr_in servaddr, cli;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        printf("socket failed to create\n");
        exit(0);
    }
    else
    {
        printf("Socket created succesfully\n");
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection failed\n");
        exit(0);
    }
    else
    {
        printf("Connected\n");
    }
    char buff[MAX];
    int n;
    while(true)
    {
        bzero(buff, sizeof(buff));
        printf("Enter a string: ");
        n = 0;
        while((buff[n] = getchar()) != '\n')
        {
            n++;
        }
        write(sockfd, buff, sizeof(buff));
        if(strncmp("exit", buff) == 0)
        {
            printf("Client exit \n");
            break;
        }

        
        // bzero(buff, sizeof(buff));
        // read(sockfd, buff, sizeof(buff));
        // printf("From server: %s", buff);
        // if(strncmp("exit", buff, 4) == 0)
        // {
        //     printf("Client exit \n");
        //     break;
        // }
    }
    close(sockfd);
}