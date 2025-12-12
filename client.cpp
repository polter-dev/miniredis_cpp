//standard import
#include <iostream>
#include <stdlib.h>
using namespace std;

//import for to grab socket / ip includes necessaryq
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

/*
This program is fully developed and created by: Marcus Ruth 
https://www.github.com/polter-dev/miniredis_cpp

Program Synopsis: client that connects and communicates to the server to simulate a redis like system
*/


//grab socket and error handles
int grabSocket(){
    int fd = socket(PF_INET, SOCK_STREAM, 0);
        if (fd == -1){
            printf("Socket could not be grabbed!\n");
            exit(errno);
        }
    return fd;
}

void initSockAddr(struct sockaddr_in *s1){
    s1->sin_family = AF_INET;
    s1->sin_port = htons(1234);
    s1->sin_addr.s_addr = inet_addr("127.0.0.1"); //local address
}

void connectSock(int fd, struct sockaddr_in *s){
    int connectNo = connect(fd, (struct sockaddr *)s, sizeof(*s));
    if (connectNo){
        printf("Connect could not be made\n");
        exit(errno);
    }
}

int main(){
    int fd = grabSocket();

    struct sockaddr_in clientSock1;
    clientSock1;
    initSockAddr(&clientSock1);

    connectSock(fd, &clientSock1);

    return 0;
}