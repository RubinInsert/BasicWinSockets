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
int main(int argc, char* argv[]) {
    // Initialize Winsock
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(res) {
        printf("WSAStartup failed: %d\n", res);
        return 1;
    }

    // Create socket
    SOCKET listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // DGRAM for UDP
    if(listener == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind socket to server address
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_pton(AF_INET, ADDRESS, &address.sin_addr); // Convert address from string to binary
    // address.sin_addr.s_addr = inet_addr(ADDRESS); // Alternative way to set the address

    POINT p;
    while(1) {
        if(GetCursorPos(&p)) {
            printf("Mouse Position: (%d, %d)\n", p.x, p.y);
        } else {
            printf("GetCursorPos failed: %d\n", GetLastError());
        }
        char buf[sizeof(POINT)];
        memcpy(buf, &p, sizeof(POINT)); // Serialize the POINT structure to send over the network
        int sendRes = sendto(listener, buf, sizeof(POINT), 0, (struct sockaddr*)&address, sizeof(address));
        if(sendRes == SOCKET_ERROR) {
            printf("Failed to send to Server: %d\n", WSAGetLastError());
            closesocket(listener);
            WSACleanup();
            return 1;
        }
    }







    // Close WinSock
    if(res == SOCKET_ERROR) {
        printf("closesocket failed: %d\n", WSAGetLastError());
    }
    WSACleanup();
}