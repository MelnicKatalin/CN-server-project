#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <iostream>
#include <string>
#define MAX 1025
#define PORT 8080
#define SA struct sockaddr


int main()
{
    int sockfd, connfd, n;
    int opt = 1;
    int new_socket, max_clients = 3, activity, valread, max_sd;
    int client_socket[3] = {0};
    int len;
    struct sockaddr_in sevaddr, cli;
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // listener socket
    char buffer[1025];
    fd_set readfds;
    std::string message = "The maximum number of clients has been reached";
    
    if(sockfd < 0)
    {
        printf("socket failed to create\n");
        exit(0);
    }
    else
    {
        printf("Socket created succesfully\n");
    }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt))< 0) {
        printf("setsockopt failed");
        exit(0);
    }

    sevaddr.sin_family = AF_INET;
    sevaddr.sin_addr.s_addr = htonl(INADDR_ANY); // local ad
    sevaddr.sin_port = htons(PORT); // most significant byte first
    
    int bindfd = bind(sockfd, (SA*)&sevaddr, sizeof(sevaddr));
    if(bindfd < 0)
    {
        printf("socket failed to bind\n");
        exit(0);
    }
    else
    {
        printf("Socket bind was succesfull\n");
    }
    int listenfd = listen(sockfd, 5);
    if(listenfd < 0)
    {
        printf("listen failed");
        exit(0);
    }
    else
    {
        printf("Server is listening\n");
    }
    
    std::cout << "Waiting for connections...\n";
    
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    max_sd = sockfd;
    
    while(true)
    {
           
        for (int i = 0; i < max_clients; i++) {
            if (client_socket[i] > 0) {
                FD_SET(client_socket[i], &readfds);
            }
            if (client_socket[i] > max_sd) {
                max_sd = client_socket[i];
            }
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            printf("select error\n");
        }

        if (FD_ISSET(sockfd, &readfds)) {
            len = sizeof(cli);
            new_socket = accept(sockfd, (SA*) &cli, (socklen_t*) &len);
            if (new_socket < 0) {
                printf("accept error");
                exit(0);
            }
            else {
                for (int i = 0; i < max_clients; i++) {
                    if (client_socket[i] == 0) {
                        client_socket[i] = new_socket;
                        printf("Adding to list of sockets as %d\n", i);
                        printf("New connection, socket is %d, id is %s, port: %d\n", new_socket, inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
                        break;
                    }
                }
                
            }
        }
        
        FD_SET(new_socket, &readfds);
        if (new_socket > max_sd) {
            max_sd = new_socket;
        }

        for (int i = 0; i < max_clients; i++) {
            
            if (FD_ISSET(client_socket[i], &readfds)) {
                n = recv(client_socket[i], buffer, MAX, MSG_DONTWAIT);
                if (n == -1) {
                    ;
                }
                else if (n == 0) {
                    getpeername(client_socket[i], (SA*) &cli, (socklen_t*) &len);
                    close(client_socket[i]);
                    FD_CLR(client_socket[i], &readfds);
                    printf("Client with ip: %s and port: %d disconnected\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
                    client_socket[i] = 0;
                }
                else {
                    getpeername(client_socket[i], (SA*) &cli, (socklen_t*) &len);
                    printf("From %s, %d: %s\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port), buffer);
                }
            }
        }     
    }
    return 0;
    
}