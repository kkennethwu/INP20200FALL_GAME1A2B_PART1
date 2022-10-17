//  ##### Final Version #####
//  server_v3.cpp
//
//
//  Created by Kenneth Wu on 2022/10/14.
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

//#define PORT 8888
#define STDIN 0
using namespace std;



struct UserInfo{
    string username;
    string email;
    string password;
    bool is_login = 0;
    
};
vector<UserInfo> user_info;

void* SocketHandler(void*);

bool find_username(string username){
    for(int i = 0; i < user_info.size(); i++){
        if(user_info[i].username == username) return true;
    }
    return false;
}

bool find_email(string email){
    for(int i = 0; i < user_info.size(); i++){
        if(user_info[i].email == email) return true;
    }
    return false;
}

int find_usersite(string username){
    for(int i = 0; i < user_info.size(); i++){
        if(user_info[i].username == username) return i;
    }
    return -1;
}


const vector<string> SplitCommand(string command){
    vector<string> result;
    string::size_type begin, end;
    string pattern = " ";

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

char zero = '0';
int rand_num;
string RandomInitilaFourDigit(){
    string tmp;
    srand((unsigned) time(NULL));
    for(int i = 0; i < 4; i++){
        rand_num = rand() % 10;
        tmp.push_back(zero + rand_num);
    }
    return tmp;
}

bool check_isnot_4digit(string tmp){
    if(tmp.size() != 4) return true;
    for(int i = 0; i < 4; i++){
        if(tmp[i] < 48 || tmp[i] > 57) return true;
    }
    return false;
}


string _1A2B(string question, string answer){
    int A = 0, B = 0;
    for(int i = 0; i < 4; i++){
        if(question[i] == answer[i]){
            A++;
            answer[i] = 'x';
            question[i] = 'x';
        }
    }
    for(int i = 0; i < 4; i++){
        if(question[i] == 'x') continue;
        for(int j = 0; j < 4; j++){
            if(answer[j] == 'x') continue;
            if(question[i] == answer[j]){
                question[i] = 'x';
                answer[j] = 'x';
                B++;
            }
        }
    }
    //
    string tmp;
    tmp.push_back((A + '0'));
    tmp.push_back('A');
    tmp.push_back((B + '0'));
    tmp.push_back('B');
                  
    return tmp;
}



int main(int argc, char *argv[]){
    char* PORT = argv[1];
    
    int serverfd_tcp, serverfd_udp, opt_tcp, opt_udp, new_socket_tcp;
    struct sockaddr_in saddr_tcp, saddr_udp;
    long data_recv_tcp;
    char buffer[1024] = { 0 };//????
    pthread_t thread_id = 0;
    
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
    saddr_tcp.sin_port = htons((unsigned short)strtoul(PORT, NULL, 0));
    int saddrlen_tcp = sizeof(saddr_tcp);
    
    saddr_udp.sin_family = AF_INET;
    saddr_udp.sin_addr.s_addr = INADDR_ANY;
    saddr_udp.sin_port = htons((unsigned short)strtoul(PORT, NULL, 0));
    int saddrlen_udp = sizeof(saddr_udp);
    /*    bind    */
    bind(serverfd_tcp, (struct sockaddr*)&saddr_tcp, sizeof(saddr_tcp));
    bind(serverfd_udp, (struct sockaddr*)&saddr_udp, sizeof(saddr_udp));
    /* tcp listening */
//    printf("Listening...\n");
    listen(serverfd_tcp, SOMAXCONN); // SOMAXCONN: mac connection number 128 define in socket.h
    
    
    /**/
    char const* welcome = "*****Welcome to Game 1A2B*****";
    char const* server_reply;
    fd_set readfds;
    long bytecount_udp;
    int max_sd;
    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));
    socklen_t len = sizeof(cliaddr);
    string command, token;
    int activity;
    vector<string> split_result;
    UserInfo userinfo_tmp;
    
    while(true){
        memset(buffer, 0, 1024);
        FD_ZERO(&readfds);
        max_sd = max(serverfd_tcp, serverfd_udp) + 1;
        FD_SET(serverfd_tcp, &readfds);
        FD_SET(serverfd_udp, &readfds);
        activity = select(max_sd + 1,&readfds, NULL, NULL, NULL);
        if ((activity < 0))
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        
        /*TCP and ACCPET*/
        if(FD_ISSET(serverfd_tcp, &readfds)){
            new_socket_tcp = accept(serverfd_tcp, (struct sockaddr*)&saddr_tcp, (socklen_t*) &saddrlen_tcp);
            if(new_socket_tcp < 0){
                perror("accept");
                exit(EXIT_FAILURE);
            }
            cout << "New connection." << endl;
            /* send welcome */
            long data_send = send(new_socket_tcp, welcome, strlen(welcome), 0);
            if(data_send < 0){
                perror("send");
                exit(EXIT_FAILURE);
            }
            /*TCP connection*/
            pthread_create(&thread_id, 0, &SocketHandler, (void*)&new_socket_tcp);
            pthread_detach(thread_id);
        }/*UDP*/
        else if(FD_ISSET(serverfd_udp, &readfds)){
            if((bytecount_udp = recvfrom(serverfd_udp, buffer, 1024, 0, (struct sockaddr * )&cliaddr, &len))== -1){
                perror("recvfrom: ");
                exit(EXIT_FAILURE);
            }
            command = buffer;
//            cout << "[client]: " << command << endl;
            /* get token */
            token = command.substr(0, command.find(" "));
            /* classify token */
            if(token == "register"){
                split_result = SplitCommand(command);
                if(split_result.size() != 4){// register <username> <email> <password>
                    server_reply = "Usage: register <username> <email> <password>\n";
                    sendto(serverfd_udp, server_reply, strlen(server_reply), 0, (const struct sockaddr *) &cliaddr, len);
                }
                else{
                    if(find_username(split_result[1])){// Fail(1)
                        server_reply = "Username is already uesd.\n";
                        sendto(serverfd_udp, server_reply, strlen(server_reply), 0, (const struct sockaddr *) &cliaddr, len);
                    }
                    else if (find_email(split_result[2])){// Fail(2)
                        server_reply = "Email is already uesd.\n";
                        sendto(serverfd_udp, server_reply, strlen(server_reply), 0, (const struct sockaddr *) &cliaddr, len);
                    }
                    else{// Sucess
                        userinfo_tmp.username = split_result[1];
                        userinfo_tmp.email  = split_result[2];
                        userinfo_tmp.password = split_result[3];
                        user_info.push_back(userinfo_tmp);
                        server_reply = "Register successfully.\n";
                        sendto(serverfd_udp, server_reply, strlen(server_reply), 0, (const struct sockaddr *) &cliaddr, len);
                    }
                }
            }
            else if(command == "game-rule"){
                server_reply = "1. Each question is a 4-digit secret number.\n2. After each guess, you will get a hint with the following information:\n2.1 The number of \"A\", which are digits in the guess that are in the correct position.\n2.2 The number of \"B\", which are digits in the guess that are in the answer but are in the wrong position.\nThe hint will be formatted as \"xAyB\".\n3. 5 chances for each question.\n";
                sendto(serverfd_udp, server_reply, strlen(server_reply), 0, (const struct sockaddr *) &cliaddr, len);
            }
            
            
            
        }
        
    }
}

void* SocketHandler(void* lp){
    int new_socket_tcp = *(int*) lp;
    long bytecount_udp, data_send;
    string command, token;
    char buffer[1024] = { 0 };
    char const* server_reply;
    vector<string> split_result;
    int user_now_index = -1;
    string answer;
    int guess_count = -1;
    
    
    while(true){
//        cout << guess_count << endl;
        memset(buffer, 0, 1024);
        if((bytecount_udp = recv(new_socket_tcp, buffer, 1024, 0))== -1){
            perror("new_socket");
            exit(EXIT_FAILURE);
        }
        command = buffer;
//        std :: cout << "[client]: " << command << std :: endl;
        /*in game-mode*/
        if(guess_count > -1){
            if(check_isnot_4digit(command)){
                server_reply = "Your guess should be a 4-digit number.\n";
                data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                if(data_send < 0){
                    perror("send");
                    exit(EXIT_FAILURE);
                }
            }
            else{//guess
                guess_count++;
//                cout << "guess++" << endl;
                string tmp_msg = _1A2B(command, answer);
                tmp_msg += "\n";
                if(tmp_msg == "4A0B\n"){
                    guess_count = -1;
                    tmp_msg = "You got the answer!\n";
                }
                else if(guess_count == 5){
                    guess_count = -1;
                    tmp_msg += "You lose the game!\n";
                }
                server_reply = tmp_msg.c_str();
                data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                if(data_send < 0){
                    perror("send");
                    exit(EXIT_FAILURE);
                }
            }
            continue;
        }
        /* get token */
        split_result = SplitCommand(command);
        token = split_result[0];
        if(token == "login"){
            if(split_result.size() != 3){//
                server_reply = "Usage: login <username> <password>\n";
                data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                if(data_send < 0){
                    perror("send");
                    exit(EXIT_FAILURE);
                }
            }
            else {
                int i = find_usersite(split_result[1]);
                if(i == -1){//Fail(2)
                    server_reply = "Username not found.\n";
                    data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                    if(data_send < 0){
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                }
                else{
                    if(user_info[i].password != split_result[2]){//Fail(3)
                        server_reply = "Password not correct.\n";
                        data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                        if(data_send < 0){
                            perror("send");
                            exit(EXIT_FAILURE);
                        }
                    }
                    else if(user_info[i].is_login == 1 || user_now_index > -1){//Fail(1)
                        server_reply = "Please logout first.\n";
                        data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                        if(data_send < 0){
                            perror("send");
                            exit(EXIT_FAILURE);
                        }
                    }
                    else{//Sucess
                        user_info[i].is_login = 1;
                        user_now_index = i;
                        string tmp_str = "Welcome, " + split_result[1] + ".\n";
                        server_reply = tmp_str.c_str();
                        data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                        if(data_send < 0){
                            perror("send");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }
            continue;
        }
        else if(command == "logout"){
            if(user_now_index == -1){
                server_reply = "Please login first.\n";
                data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                if(data_send < 0){
                    perror("send");
                    exit(EXIT_FAILURE);
                }
            }
            else{
                user_info[user_now_index].is_login = 0;
                string tmp_str = "Bye, " + user_info[user_now_index].username + ".\n";
                server_reply = tmp_str.c_str();
                data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                if(data_send < 0){
                    perror("send");
                    exit(EXIT_FAILURE);
                }
                user_now_index = -1;
            }
            continue;
        }
        else if(token == "start-game"){
            if((split_result.size() > 2)){//Fail(2)
                server_reply = "Usage: start-game <4-digit number>\n";
                data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                if(data_send < 0){
                    perror("send");
                    exit(EXIT_FAILURE);
                }
            }
            else{
                if(user_now_index == -1){//Fail(1)
                    server_reply = "Please login first.\n";
                    data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                    if(data_send < 0){
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                }
                else{
                    if(split_result.size() == 1){//start without parameter
                        answer = RandomInitilaFourDigit();
//                        cout << "answer: " << answer << endl;
                        guess_count = 0;
                        server_reply = "Please typing a 4-digit number:\n";
                        data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                        if(data_send < 0){
                            perror("send");
                            exit(EXIT_FAILURE);
                        }
                    }
                    else if (split_result.size() == 2){//start with parameter
                        if(check_isnot_4digit(split_result[1])){
                            server_reply = "Usage: start-game <4-digit number>\n";
                            data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                            if(data_send < 0){
                                perror("send");
                                exit(EXIT_FAILURE);
                            }
                        }
                        else{
                            answer = split_result[1];
                            guess_count = 0;
                            server_reply = "Please typing a 4-digit number:\n";
                            data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
                            if(data_send < 0){
                                perror("send");
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                }
                
            }
            continue;
        }
        else if (command == "exit"){
            if(user_now_index != -1){
                user_info[user_now_index].is_login = 0;
                user_now_index = -1;
            }
//            cout << "receive exit" << endl;
            break;
        }
//        else if (guess_count > -1){// have start game
//            cout << "have strt-game" << endl;
//            if(check_isnot_4digit(command)){
//                server_reply = "Your guess should be a 4-digit number.\n";
//                data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
//                if(data_send < 0){
//                    perror("send");
//                    exit(EXIT_FAILURE);
//                }
//            }
//            else{//guess
//                guess_count++;
//                cout << "guess++" << endl;
//                string tmp_msg = "in guess\n";
//                if(guess_count == 5){
//                    guess_count = -1;
//                     tmp_msg += "You lose the game!\n";
//                }
//
//                server_reply = tmp_msg.c_str();
//                data_send = send(new_socket_tcp, server_reply, strlen(server_reply), 0);
//                if(data_send < 0){
//                    perror("send");
//                    exit(EXIT_FAILURE);
//                }
//            }
//            continue;
//        }
    }
//    cout << "pass" << endl;
    
    
    close(new_socket_tcp);
    
    return 0;
}
    
    


