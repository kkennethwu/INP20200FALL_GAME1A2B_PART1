//  ##### Single client test #####
//  server.cpp
//  
//
//  Created by Kenneth Wu on 2022/10/11.
//

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#define PORT 8888


int main(){
    char buffer[1024] = { 0 };
    int new_socket;
    //create socket
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == - 1){
        perror("Fail to create socket");
        exit(EXIT_FAILURE);
    }
    //
    int opt = 1;
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("socletopt");
        exit(EXIT_FAILURE);
    }
    // socket info
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(PORT);
    int saddrlen = sizeof(saddr);
    //bind
    bind(serverfd, (struct sockaddr*)&saddr, sizeof(saddr));
    //listen
    printf("Listening...\n");
    listen(serverfd, SOMAXCONN); // SOMAXCONN: mac connection number 128 define in socket.h
    //accept
    
    
    new_socket = accept(serverfd, (struct sockaddr*)&saddr, (socklen_t*) &saddrlen);
    if(new_socket < 0 ){
        perror("new_socket");
        exit(EXIT_FAILURE);
    }
    printf("Connected\n");
    //
    
    long data_recv;
    while(true){
        data_recv = read(new_socket, buffer, 1024);
        std :: cout << data_recv << std :: endl;
        printf("%s\n", buffer);
        //
        char const* hello = "Hello from server";
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
    }
    
    
    //
    close(new_socket);
    shutdown(serverfd, SHUT_RDWR);
    return 0;
}


