//standard import
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <vector>
using namespace std;

//import for to grab socket / ip includes necessaryq
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

//data structure import && creation
#include <unordered_map>
#include <mutex>

//create a globally accessed map (not thread based)for all clients to access
std::unordered_map<std::string, std::string> store;
std::mutex storeMutex;

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

struct pollfd initPoll(int fd){
    struct pollfd s;
    s.fd = fd;
    s.events = POLLIN;
    s.revents = 0;
    return s;
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

int sendMessage(int fd, const char *buffer, int size){
    int sent = send(fd, buffer, size, 0);
    if (sent == -1){
        cout << "Could not send!\n" << endl;
        return 0;
    }
    return 1;
}

int checkSize(int size){
    if (size == -1){
        cout << "Could not get total size\n" << endl;
        return 0;
    } else if (size == 0){
        cout << "Client has disconnected!\n" << endl;    
        return 0;
    }
    return 1;
}

void storeSET(string wordOne, string wordTwo){
    storeMutex.lock();
    store[wordOne] = wordTwo;
    storeMutex.unlock();
}

int simpleStringResponse(int fd, const char *word){
    int len = strlen(word);
    int totalSize = len + 4;// \r\n and +1 for the null term
    char buffer[totalSize]; 

        int i;
    for (i = 0; i < len; i++)
        buffer[i + 1] = word[i];
        
    buffer[0] = '+';
    buffer[totalSize - 3] = '\r';
    buffer[totalSize - 2] = '\n';
    buffer[totalSize - 1] = '\0';

    return sendMessage(fd, buffer, totalSize-1);
}

int sendBulkString(int fd, const char *word){
    int len = strlen(word);
    string response = "$" + std::to_string(len) + "\r\n" + word + "\r\n";
    return sendMessage(fd, response.c_str(), response.length());
}

int sendNull(int fd){
    string response = "_\r\n";
    return sendMessage(fd, response.c_str(), response.length());
}

int sendSimpleError(int fd, const char *error){
    //-Error message\r\n -> different message for each 
    string response = string("-") + error + "\r\n";
    return sendMessage(fd, response.c_str(), response.length());
}

void grabGET(string access, int fd) {
    storeMutex.lock();
    if (store.find(access) == store.end()){
        storeMutex.unlock();
        sendNull(fd);
        return;
    } else{
    string send = store[access];
    storeMutex.unlock();
    sendBulkString(fd, send.c_str());
    }
}

std::vector <string> decodeClient(char* encoded){
    std::vector<string> incoming;
    int posi = 0;

    char *finder = strchr(encoded + posi, '\r');
    string temp(encoded + 1, finder);
    int size = stoi(temp);

    posi = (finder-encoded) + 2;

    for (int i = 0; i < size; i++){
        finder = strchr(encoded + posi, '\r');
        string temp(encoded + posi + 1, finder);
        int wordLen = stoi(temp);

        posi = (finder - encoded) + 2;
        string word(encoded + posi, wordLen);

        posi = posi + wordLen + 2;
        incoming.push_back(word);
    }
    return incoming;
}

int runCommands(int fd){
    int x = 1;
    int size = 1024;
    char buffer[size];

    int totalSize = recv(fd, buffer, size, 0);
    x = checkSize(totalSize);
    if (!x) return 0;

    buffer[totalSize] = '\0';
    cout << buffer << endl;

    std::vector<string> words = decodeClient(buffer);
    if (words.empty()) return sendSimpleError(fd, "ERR empty command");

    if (words[0] == "PING"){
        x = simpleStringResponse(fd, "PONG");
    } else if (words[0] == "ECHO"){
        x = sendBulkString(fd, words[1].c_str());
    } else if (words[0] == "QUIT"){
        x = 0;
    } else if (words[0] == "SET"){
        storeSET(words[1], words[2]);
        x = simpleStringResponse(fd, "OK");
    } else if (words[0] == "GET"){
        grabGET(words[1], fd);
    } else {
        x = sendSimpleError(fd, "ERR unknown command");
    }

    return x;
}

    void runPolling(std::vector<pollfd> &fdArr, int fd, sockaddr_in &serverSocket){
        while(1){
            poll(fdArr.data(), fdArr.size(), -1);
            for (int i = 0; i < fdArr.size(); i++) {
                if(fdArr[i].revents){
                    if (!i){
                        int newFD = acceptSocket(fd, &serverSocket);
                        fdArr.push_back(initPoll(newFD));
                    } else {
                        int val = runCommands(fdArr[i].fd);
                        if (!val || val == -1){
                            close(fdArr[i].fd);
                            fdArr.erase(fdArr.begin() + i);
                            i--;
                        }
                    }
                }
            }
        }
    }

    int main(){
        int fd = grabSocket();
        struct sockaddr_in serverSocket; initServerSocket(&serverSocket);
        bindSock(fd, &serverSocket);
        listenSocket(fd);


        //creates a dynamic system of fd
        std::vector<pollfd> fdArr;
        fdArr.push_back(initPoll(fd));
        
        runPolling(fdArr, fd, serverSocket);

        close(fd);
        return 0;
}