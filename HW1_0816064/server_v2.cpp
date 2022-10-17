//  ##### Multi-Thread Version but have some problem in UDP server #####
//  server_v2.cpp
//  HW1_0816064
//
//  Created by Kenneth Wu on 2022/10/12.
//

#include <stdio.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <pthread.h>
#include <sys/types.h>

//#include <fcntl.h>
#define PORT 8888
#define STDIN 0
using namespace std;
void* SocketHandler(void*);



struct SockAttr{
    int csock_tcp;
    int csock_udp;
};


int main(){
    int serverfd_tcp, serverfd_udp, opt_tcp, opt_udp, new_socket_tcp, new_socket_udp;
    struct sockaddr_in saddr_tcp, saddr_udp;
    long data_recv_tcp;
    char buffer[1024] = { 0 };//????
    pthread_t thread_id = 0;
    SockAttr* sock_attr = new SockAttr;
    
    /* socket creation */
    serverfd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd_tcp == - 1){
        perror("Fail to create socket");
        exit(EXIT_FAILURE);
    }
    serverfd_udp = socket(AF_INET, SOCK_DGRAM,  0);
    if (serverfd_udp == - 1){
        perror("Fail to create socket");
        exit(EXIT_FAILURE);
    }
    /* setsokcetopt */
    if (setsockopt(serverfd_tcp, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_tcp, sizeof(opt_tcp))){
        perror("socletopt");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(serverfd_udp, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_udp, sizeof(opt_udp))){
        perror("socletopt");
        exit(EXIT_FAILURE);
    }
    /* socket info */
    
    saddr_tcp.sin_family = AF_INET;
    saddr_tcp.sin_addr.s_addr = INADDR_ANY;
    saddr_tcp.sin_port = htons(PORT);
    int saddrlen_tcp = sizeof(saddr_tcp);
    
    saddr_udp.sin_family = AF_INET;
    saddr_udp.sin_addr.s_addr = INADDR_ANY;
    saddr_udp.sin_port = htons(PORT);
    int saddrlen_udp = sizeof(saddr_udp);
    /*    bind    */
    bind(serverfd_tcp, (struct sockaddr*)&saddr_tcp, sizeof(saddr_tcp));
    bind(serverfd_udp, (struct sockaddr*)&saddr_udp, sizeof(saddr_udp));
    /* tcp listening */
    printf("Listening...\n");
    listen(serverfd_tcp, SOMAXCONN); // SOMAXCONN: mac connection number 128 define in socket.h
    /**/
    while(true){
        sock_attr->csock_tcp = accept(serverfd_tcp, (struct sockaddr*)&saddr_tcp, (socklen_t*) &saddrlen_tcp);
        cout << "New connection" << endl;
        if(sock_attr->csock_tcp < 0 ){
            perror("accpet");
            exit(EXIT_FAILURE);
        }
        sock_attr->csock_udp = serverfd_udp;
        pthread_create(&thread_id, 0, &SocketHandler, (void*)sock_attr);
        pthread_detach(thread_id);
        
    }
    
}

// （更改法一）創多個udp client


void* SocketHandler(void* lp){
    SockAttr sock_attr = *(SockAttr*) lp;
    char const* server_message;
    char const* server_reply = "[server] received commad";
    char const* welcome = "*****Welcome to Game 1A2B*****";
    
    string command;
    string token;
    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));
    
    char buffer[1024] = {};
    int buffer_len = 1024;
    long bytecount_tcp, bytecount_udp;
    socklen_t len = sizeof(cliaddr);
    
    fd_set readfds;
    int max_sd;
    
    /*send welcome*/
    long data_send = send(sock_attr.csock_tcp, welcome, strlen(welcome), 0);
    if(data_send < 0){
       perror("send");
       exit(EXIT_FAILURE);
    }
    pthread_t tid = pthread_self();
    cout << "thread_id: " << tid << endl;
    cout << "tcp: " << sock_attr.csock_tcp << " udp: " << sock_attr.csock_udp << endl;
    
    /*send welcome*/
   
    
    
    
    while(true){
        memset(buffer, 0, buffer_len);
        /* SELECT UDP/TCP */
        FD_ZERO(&readfds);
        max_sd = max(sock_attr.csock_tcp, sock_attr.csock_udp) + 1;
        FD_SET(sock_attr.csock_tcp, &readfds);
        FD_SET(sock_attr.csock_udp, &readfds);
        cout << "stuck at select; sock_attr => tcp: " << sock_attr.csock_tcp << " udp: " << sock_attr.csock_udp << endl;
        int activity = select(max_sd + 1,&readfds, NULL, NULL, NULL);
        cout << "t_id: " << tid << " tcp: " << sock_attr.csock_tcp << " udp: " << sock_attr.csock_udp << " activity: " << activity <<endl;
        if ((activity < 0))
        {
            perror("select");
            exit(-1);
        }
        cout << tid << "passA" << endl;
        /*Use UDP or TCP*/
        if(FD_ISSET(sock_attr.csock_udp, &readfds)){
            /* recv */
            if((bytecount_udp = recvfrom(sock_attr.csock_udp, buffer, buffer_len, 0, (struct sockaddr * )&cliaddr, &len))== -1){
                perror("recvfrom: ");
                exit(EXIT_FAILURE);
            }
            command = buffer;
            cout << "[client]: " << command << endl;
            /* get token */
            token = command.substr(0, command.find(" "));
            cout << "token: " << token << endl;
            /* classify token */
            if(token == "register"){
                if(command == "register"){
                    server_message = "Usage:register <username> <email> <password>\n";
                    sendto(sock_attr.csock_udp, server_message, strlen(server_message), 0, (const struct sockaddr *) &cliaddr, len);
                }
                
                
            }
            else if(token == "game-rule"){
                server_message = "1. Each question is a 4-digit secret number.\n2. After each guess, you will get a hint with the following information:\n2.1 The number of \"A\", which are digits in the guess that are in the correct position.\n2.2 The number of \"B\", which are digits in the guess that are in the answer but are in the wrong position.\nThe hint will be formatted as \"xAyB\".\n3. 5 chances for each question.\n";
                sendto(sock_attr.csock_udp, server_message, strlen(server_message), 0, (const struct sockaddr *) &cliaddr, len);
            }
        }
        else if (FD_ISSET(sock_attr.csock_tcp, &readfds)){
            if((bytecount_udp = recv(sock_attr.csock_tcp, buffer, buffer_len, 0))== -1){
                perror("new_socket");
                exit(EXIT_FAILURE);
            }
            command = buffer;
            std :: cout << "[client]: " << command << std :: endl;
            /* get token */
            token = command.substr(0, command.find(" "));
//            cout << "token: " << token << endl;
//            char const* hello = "[server]: message received\n";
//            send(sock_attr->csock_tcp, hello, strlen(hello), 0);
            /* classify token */
            if(token == "login"){
                long data_send = send(sock_attr.csock_tcp, server_reply, strlen(server_reply), 0);
                if(data_send < 0){
                    perror("send");
                    exit(EXIT_FAILURE);
                }
            }
            else if (token == "exit"){
                
                break;
            }
            cout << tid << "passB" << endl;
        }
        
       
        
        
        
        
        
    }
    
    
    
    close(sock_attr.csock_tcp);
    close(sock_attr.csock_udp);
    printf("csock closed\n");
    return 0;
}
