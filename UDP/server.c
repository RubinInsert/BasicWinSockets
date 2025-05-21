#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define ADDRESS "127.0.0.1"
#define PORT 27015

#define BUFLEN 512
int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(res) {
        printf("WSAStartup failed: %d\n", res);
        return 1;
    }
    // Create socket (Listener)
    SOCKET listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // DGRAM for UDP
    if(listener == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    // Bind the socket to an address
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(ADDRESS);
    res = bind(listener, (struct sockaddr*)&address, sizeof(address)); // Bind the general socket to a specific address (the server)
    if(res == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }
    printf("Listening on %s:%d\n", ADDRESS, PORT);

    struct sockaddr_in clientAddr;
    ZeroMemory(&clientAddr, sizeof(clientAddr));
    char buf[BUFLEN]; // Buffer for the message
    

    while (1) {
        ZeroMemory(buf, BUFLEN);
        // Wait for a message
        int clientAddrLen = sizeof(clientAddr);
        POINT p;
        int bytesIn = recvfrom(listener, (char*)&p, sizeof(POINT), 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (bytesIn == SOCKET_ERROR) {
            printf("Error recieving from client %d\n", WSAGetLastError());
            continue; // dont break the loop
        }
        // Display the message
        char clientIP[256];
        ZeroMemory(clientIP, 256);
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
        printf("Message from (%s:%d) : Mouse Position(%ld, %ld)\n", clientIP, ntohs(clientAddr.sin_port), p.x, p.y);
    }







    // Shudown Winsock
    res = closesocket(listener);
    if(res == SOCKET_ERROR) {
        printf("closesocket failed: %d\n", WSAGetLastError());
    }
    WSACleanup();
}