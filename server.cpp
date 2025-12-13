//standard import
#include <iostream>
#include <stdlib.h>
#include <string.h>
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
int grabSocket(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1){
        cout << "Could not grab server socket" << endl;
        exit(errno);
    }
    return fd;
}

void initServerSocket(struct sockaddr_in *s1){
    s1->sin_family = AF_INET;
    s1->sin_port = htons(1234);
    s1->sin_addr.s_addr = htonl(INADDR_ANY);
}

void bindSock(int fd, struct sockaddr_in *serverSocket){
    int res = ::bind(fd, (struct sockaddr *)serverSocket, sizeof(*serverSocket));
    if (res == -1){
        cout << "Could not bind to socket!\n" << endl;
        exit(-1);
    }
}

void listenSocket(int fd){
    int val = listen(fd, 1);
    if (val == -1){
        cout << "Something went wrong with listening\n" << endl;
        exit(errno);
    }
}

int acceptSocket(int fd, struct sockaddr_in *clientAddr){
    socklen_t num =sizeof(*clientAddr);
    int res = accept(fd, (struct sockaddr *)clientAddr, &num);
    if (res == -1){
        cout << "Connection could not be accepting through server\n" << endl;
        exit(errno);
    }
    return res;
}

int main(){
    int fd = grabSocket();
    struct sockaddr_in serverSocket; initServerSocket(&serverSocket);
    bindSock(fd, &serverSocket);
    listenSocket(fd);

    while (1){
        int newFd = acceptSocket(fd, &serverSocket);
        int size = 1024;
        char buffer[size];
        int totalSize = recv(newFd, buffer, size, 0);
        if (totalSize == -1){
            cout << "Could not get total size\n" << endl;    
            break;
        } else if (totalSize==0){//client closed connection
            cout << "Client has closed connection\n" << endl;
            close(newFd);
        } else {
    
        buffer[totalSize] = '\0';

        cout << buffer << endl;
        int sent = send(newFd, buffer, totalSize, 0);
        if (sent == -1){
            cout << "Could not send\n" << endl;
            break;
        }
        close(newFd);
        }
    }

    close(fd);
    return 0;
}