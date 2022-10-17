//  ##### Final Version #####
//  client_v3.cpp
//  
//
//  Created by Kenneth Wu on 2022/10/14.
//

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <vector>
//#define PORT 8888
using namespace std;

const vector<string> SplitCommand(string command){
    vector<string> result;
    string::size_type begin, end;
    string pattern = "\n";

    end = command.find(pattern);
    begin = 0;

    while (end != std::string::npos) {
        if (end - begin != 0) {
            result.push_back(command.substr(begin, end-begin));
        }
        begin = end + pattern.size();
        end = command.find(pattern, begin);
    }

    if (begin != command.length()) {
        result.push_back(command.substr(begin));
    }
    
    return result;
}

int main(int argc, char *argv[]){
    char* PORT = argv[2];
    
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
    caddr_tcp.sin_port = htons((unsigned short)strtoul(PORT, NULL, 0));
    caddr_udp.sin_family = AF_INET;
    caddr_udp.sin_port = htons((unsigned short)strtoul(PORT, NULL, 0));
    
    //
    if(inet_pton(AF_INET, argv[1], &caddr_tcp.sin_addr) <= 0){
        printf("\n TCP Invalid address/ Address not supported \n");
        return -1;
    }
    if(inet_pton(AF_INET, argv[1], &caddr_udp.sin_addr) <= 0){
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
    /* welcome */
    memset(buffer, 0, 1024);
    read(sock_tcp, buffer, 1024);
    printf("%s\n", buffer);
    
    std :: string command, token, return_msg;
    char const* tmp;
    long data_send;
    socklen_t len = sizeof(caddr_udp);
    bool game_mode = 0;
    vector<string> split_msg;
    while(true){
        memset(buffer, 0, 1024);
        //get command
        getline(std :: cin, command);
        tmp = command.c_str();
        token = command.substr(0, command.find(" "));
//        cout << "command: " << command << endl;
//        cout << "token: " << token << endl;
        /* in game-mode */
//        cout << "game_mode: " << game_mode << endl;
        if (game_mode == 1){
            
            data_send = send(sock_tcp, tmp, strlen(tmp), 0);
            if(data_send < 0){
                perror("sned");
                exit(EXIT_FAILURE);
            }
            read(sock_tcp, buffer, 1024);
            printf("%s", buffer);
            return_msg = buffer;
//            cout << "return_msg: " << return_msg << endl;
            split_msg = SplitCommand(return_msg);
//            cout << "split_msg: " << split_msg[0] << " " << split_msg[1] << endl;
//            cout << "split_msg.size: " << split_msg.size() << endl;
            if(split_msg.size() == 1 && split_msg[0] == "You got the answer!"){
                game_mode = 0;
            }
            if(split_msg.size() == 2 && split_msg[1] == "You lose the game!"){
                game_mode = 0;
            }
            
            continue;
        }
        /* UDP */
        if(token == "register"){
            data_send = sendto(sock_udp, tmp, strlen(tmp), 0, (struct sockaddr *) &caddr_udp, sizeof(caddr_udp));
            if(data_send < 0){
                perror("sned");
                exit(EXIT_FAILURE);
            }
            recvfrom(sock_udp, buffer, 1024, 0, (struct sockaddr *) &caddr_udp, &len);
            printf("%s", buffer);
        }
        else if (command == "game-rule"){
//            cout << "pass" << endl;
            data_send = sendto(sock_udp, tmp, strlen(tmp), 0, (struct sockaddr *) &caddr_udp, sizeof(caddr_udp));
            if(data_send < 0){
                perror("sned");
                exit(EXIT_FAILURE);
            }
            recvfrom(sock_udp, buffer, 1024, 0, (struct sockaddr *) &caddr_udp, &len);
            printf("%s", buffer);
        }
        /* TCP */
        else if (token == "login"){
            data_send = send(sock_tcp, tmp, strlen(tmp), 0);
            if(data_send < 0){
                perror("sned");
                exit(EXIT_FAILURE);
            }
            read(sock_tcp, buffer, 1024);
            printf("%s", buffer);
        }
        else if(command == "logout"){
            data_send = send(sock_tcp, tmp, strlen(tmp), 0);
            if(data_send < 0){
                perror("sned");
                exit(EXIT_FAILURE);
            }
            read(sock_tcp, buffer, 1024);
            printf("%s", buffer);
        }
        else if (token == "start-game"){
            data_send = send(sock_tcp, tmp, strlen(tmp), 0);
            if(data_send < 0){
                perror("sned");
                exit(EXIT_FAILURE);
            }
            read(sock_tcp, buffer, 1024);
            printf("%s", buffer);
            return_msg = buffer;
            if(return_msg == "Please typing a 4-digit number:\n"){
                game_mode = 1;
            }
//            cout << "game_mode changed to " << game_mode << endl;
            
        }
        else if(command == "exit"){
            data_send = send(sock_tcp, tmp, strlen(tmp), 0);
            if(data_send < 0){
                perror("sned");
                exit(EXIT_FAILURE);
            }
            break;
        }

        //
    }
    //
    close(client_fd_tcp);
    close(sock_tcp);
    close(sock_udp);
    return 0;
}

