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

int sendText(const char *word, int fd){
    int val = send(fd, word, strlen(word), 0);
    if (val == -1){
        printf("There was an issue sending!\n");
        exit(errno);
    }
    return val;
}

char *setWord(char input[1024]){
    int len = strlen(input);
    char *ret = (char*)malloc(sizeof(char) * len+1);
    strcpy(ret, input);
    return ret;
}

int clientRun(int fd){
    char userBuffer[1024];
    fgets(userBuffer, 1024, stdin);
    
    char *temp = setWord(userBuffer);
    int sent = sendText(temp, fd);
    if (!strncmp(temp, "QUIT", 4)){
        free(temp);
        return 0;
    }
    free(temp);

    char *buffer = (char*)malloc(sizeof(char) * 1024 + 1);
    int received = recv(fd, buffer, 1024, 0);
    if (received == -1){
        printf("Issue receiving size of text in bytes\n");
        exit(-1);
    } else if (received == 0){
        printf("TCP connection was closed on server side before processing \n");
        exit(0);
    }
    
    buffer[received] = '\0';
    cout << buffer << endl;
    free(buffer);

    return 1;
}

int main(){
    int fd = grabSocket();

    struct sockaddr_in clientSock1;
    initSockAddr(&clientSock1);

    connectSock(fd, &clientSock1);

    int x = 1;
    cout << "Input commands\n" << endl;

    while(x){
        x = clientRun(fd);
    }
    
    close(fd);
    return 0;
}