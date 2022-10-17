# Intro. to Network Programming 2022 Fall
## Homework 1 – Game 1A2B: Part 1

###  Description
In this project, you are asked to use only C/C++ to design a 1A2B game server and client. Your program should be able to handle multiple connections and receive user commands from standard input. After receiving the command, the server sends the corresponding message back.
The logic of this project:
Client to do: Receive message from User -> Send to Server -> Receive response from Server -> Show the response Server to do: Receive message from Client -> Do corresponding work -> Send response to Client

### Main idea
#### server.cpp
* Create a TCP socket and a UDP scoekt.
* Do setsockopt(), intput sockinfo, bind for TCP socket and UDP socket.
* Use a while loop to :
    * Use **select()** to chose which of the following two statement to perform.
    1. Keep accepting client to connect to the TCP socket, and if connected, create a thread to handling this client.
    2. Kepp receive new message through UDP socket using **recvfrom** and sned back to the client using **sendto**
    
### client.cpp
* * Create a TCP socket and a UDP scoekt.
* Do setsockopt(), intput sockinfo for TCP socket and UDP socket.
* Do connect for TCP socket.
* Use a while loop to communicate with server.
    * TCP use send, recv;
    * UDP use sendto, recvfrom.

