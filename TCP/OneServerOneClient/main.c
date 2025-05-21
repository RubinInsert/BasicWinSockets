#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFLEN 512
#define PORT 27015
#define ADDRESS "127.0.0.1"
int main() {
    printf("Hello, World!\n");
    int res;
    // Initialize Winsock ======================================
    WSADATA wsaData; // Configuration
    res = WSAStartup(MAKEWORD(2, 2), &wsaData); 
    if (res) { // Failure with the startup of Winsock
        printf("WSAStartup failed: %d\n", res);
        return 1;
    }
    // ===========================================================
    // SETUP SERVER ============================================
    SOCKET listener;
    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) { // Failure with the socket creation
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //  Bind socket to address
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(ADDRESS);
    res = bind(listener, (struct sockaddr*)&address, sizeof(address));
    if (res == SOCKET_ERROR) { // Failure with the bind
        printf("bind failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    // Set as a listener
    res = listen(listener, SOMAXCONN); // SOMAXCONN is the maximum number of connections
    if (res == SOCKET_ERROR) { // Failure with the listen
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }
    printf("Listening on %s:%d\n", ADDRESS, PORT);


    // HANDLE A CLIENT ==========================================
    SOCKET client;
    struct sockaddr_in client_addr;
    int addrlen;
    client = accept(listener, NULL, NULL);
    if(client == INVALID_SOCKET) { // Failure with the accept
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }
    // get client info
    getpeername(client, (struct sockaddr*)&client_addr, &addrlen);
    printf("Client Connected at %s: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Send a message
    int sendRes;
    char *welcome = "Welcome to the server!";
    sendRes = send(client, welcome, strlen(welcome), 0);
    if(sendRes != strlen(welcome)) {
        printf("send failed: %d\n", WSAGetLastError());
        shutdown(client, SD_BOTH);
        closesocket(client);
        WSACleanup();
        return 1;
    }
    char recvbuf[BUFLEN];
    int recvBufLen = BUFLEN;
    do {
        res = recv(client, recvbuf, recvBufLen, 0);
        if (res > 0) {
            printf("Message Recieved (%d): %s\n", res, recvbuf);
            recvbuf[res] = '\0'; // Null terminate the string
            if(!memcmp(recvbuf, "/quit", 5 * sizeof(char))) {
                printf("Closing connection. \n");
                break;
            }

            sendRes = send(client, recvbuf, res, 0);
            if(sendRes != res) {
                printf("Error sending: %d\n", WSAGetLastError());
                shutdown(client, SD_BOTH);
                closesocket(client);
                break;
            }
        } else if(!res) {
            printf("Closing connection. \n");
            break;
        }
        else {
            printf("Receive failed: %d\n", WSAGetLastError());
            closesocket(client);
            shutdown(client, SD_BOTH);
        }
    } while (res > 0);
    // Shutdown the client
    res = shutdown(client, SD_BOTH);
    if(res == SOCKET_ERROR) { // Failure with the shutdown
        printf("shutdown failed: %d\n", WSAGetLastError());
        
    }
    closesocket(client);
    // CLEANUP - After using Winsock =============================
    // Shut down server socket
    closesocket(listener);
    // cleanup WSA
    res = WSACleanup();
    if (res) { // Failure with the cleanup of Winsock
        printf("WSACleanup failed: %d\n", res);
        return 1;
    }
    printf("Shut down successful");
    return 0;
}