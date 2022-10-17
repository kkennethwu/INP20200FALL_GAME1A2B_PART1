//
//  client.cpp
//  
//
//  Created by Kenneth Wu on 2022/10/11.
//

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#define PORT 8888

int main(){
    char buffer[1024] = { 0 };
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        printf("\n Socket creation error \n");
        return -1;
    }    //
    struct sockaddr_in caddr;
    caddr.sin_family = AF_INET;
    caddr.sin_port = htons(PORT);
    //
    if(inet_pton(AF_INET, "127.0.0.1", &caddr.sin_addr) <= 0){
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    //
    int client_fd = connect(sock, (struct sockaddr*) &caddr, sizeof(caddr));
    if(client_fd < 0){
        printf("Connection Failed\n");
        return -1;
    }
    //
    std :: string command;
    char const* tmp;
    long data_send;
    while(true){
        getline(std :: cin, command);
        tmp = command.c_str();
        data_send = send(sock, tmp, strlen(tmp), 0);
        if(data_send < 0){
            printf("Send Failed\n");
            return -1;
        }
        printf("Command sent\n");
        //
        long data_recv = read(sock, buffer, 1024);
        printf("%s\n", buffer);
        
        if(command == "exit") break;
    }
    printf("out of while loop\n");
    
    //
    close(client_fd);
    
    return 0;
}
