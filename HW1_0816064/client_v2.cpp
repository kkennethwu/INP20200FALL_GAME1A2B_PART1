// ##### Multi-Thread Version but have some problem in UDP server #####
//  client_v2.cpp
//
//
//  Created by Kenneth Wu on 2022/10/12.
//

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#define PORT 8888
using namespace std;

int main(){
    int sock_tcp, sock_udp;
    struct sockaddr_in caddr_tcp, caddr_udp;
    char buffer[1024] = { 0 };
    //
    sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_tcp < 0){
        printf("\n TCP Socket creation error \n");
        return -1;
    }
    sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_udp < 0){
        printf("\n UDP Socket creation error \n");
        return -1;
    }
    //
    caddr_tcp.sin_family = AF_INET;
    caddr_tcp.sin_port = htons(PORT);
    caddr_udp.sin_family = AF_INET;
    caddr_udp.sin_port = htons(PORT);
    
    //
    if(inet_pton(AF_INET, "127.0.0.1", &caddr_tcp.sin_addr) <= 0){
        printf("\n TCP Invalid address/ Address not supported \n");
        return -1;
    }
    if(inet_pton(AF_INET, "127.0.0.1", &caddr_udp.sin_addr) <= 0){
        printf("\n UDP Invalid address/ Address not supported \n");
        return -1;
    }
    // connect
    int client_fd_tcp = connect(sock_tcp, (struct sockaddr*) &caddr_tcp, sizeof(caddr_tcp));
    if(client_fd_tcp < 0){
        perror("tcp connection: ");
        return -1;
    }
//    int client_fd_udp = connect(sock_udp, (struct sockaddr*) &caddr_udp, sizeof(caddr_udp));
//    if(client_fd_udp < 0){
//        perror("udp connection: ");
//        return -1;
//    }
    //
    memset(buffer, 0, 1024);
    read(sock_tcp, buffer, 1024);
    printf("%s\n", buffer);
    
    std :: string command, token;
    char const* tmp;
    long data_send;
    socklen_t len = sizeof(caddr_udp);
    while(true){
        memset(buffer, 0, 1024);
        //get command
        getline(std :: cin, command);
        tmp = command.c_str();
        token = command.substr(0, command.find(" "));
        //
        if(token == "register"){//udp
            data_send = sendto(sock_udp, tmp, strlen(tmp), 0, (struct sockaddr *) &caddr_udp, sizeof(caddr_udp));
            if(data_send < 0){
                printf("Send Failed\n");
                return -1;
            }
            recvfrom(sock_udp, buffer, 1024, 0, (struct sockaddr *) &caddr_udp, &len);
            printf("%s", buffer);
        }
        else if (token == "game-rule"){//udp
            data_send = sendto(sock_udp, tmp, strlen(tmp), 0, (struct sockaddr *) &caddr_udp, sizeof(caddr_udp));
            if(data_send < 0){
                printf("Send Failed\n");
                return -1;
            }
            recvfrom(sock_udp, buffer, 1024, 0, (struct sockaddr *) &caddr_udp, &len);
            printf("%s", buffer);
        }
        else if (token == "login"){
            data_send = send(sock_tcp, tmp, strlen(tmp), 0);
            if(data_send < 0){
                printf("Send Failed\n");
                return -1;
            }
            cout << "send pass" << endl;
            read(sock_tcp, buffer, 1024);
            printf("%s\n", buffer);
        }
        else if(token == "logout"){
            data_send = send(sock_tcp, tmp, strlen(tmp), 0);
            if(data_send < 0){
                printf("Send Failed\n");
                return -1;
            }
            read(sock_tcp, buffer, 1024);
        }
        else if (token == "start-game"){
            data_send = send(sock_tcp, tmp, strlen(tmp), 0);
            if(data_send < 0){
                printf("Send Failed\n");
                return -1;
            }
            read(sock_tcp, buffer, 1024);
        }
        else if(command == "exit"){
            data_send = send(sock_tcp, tmp, strlen(tmp), 0);
            if(data_send < 0){
                printf("Send Failed\n");
                return -1;
            }
            break;
        }
        //
    }
    //
    close(client_fd_tcp);
    return 0;
}
