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
#define MAX_CLIENTS 10
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
    // Setup for multiple connections
    char multiple = !0;
    res = setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &multiple, sizeof(multiple));
    if (res < 0) { // Failure with the setsockopt
        printf("Multiple Client setup failed: %d\n", WSAGetLastError());
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

    // MAIN LOOP ==========================================
    // variables
    fd_set clientSet; // Set of clients
    SOCKET clients[MAX_CLIENTS]; // Array of clients
    SOCKET sd, max_sd; // Placeholder socket
    int currentNoSockets = 0, maxIdx = -1; // info about the array
    struct sockaddr_in clientAddr;
    int clientAddrlen;
    char running = !0;

    char recvbuf[BUFLEN];

    int sendRes;
    char* welcomeStr = "Welcome to the server!\n";
    int welcomeStrLen = strlen(welcomeStr);
    char* quitStr = "Goodbye!\n";
    int quitStrLen = strlen(quitStr);
    memset(clients, 0, MAX_CLIENTS * sizeof(SOCKET)); // Initialize the array of clients
    while(running) {
        FD_ZERO(&clientSet); // Clear the set
        FD_SET(listener, &clientSet); // Add the listener to the set
        for(int i = 0; i < MAX_CLIENTS; i++) { // ITERATE THROUGH CLIENTS
            sd = clients[i];
            if(sd > 0) {
                // Add active client to the set
                FD_SET(sd, &clientSet); // Add the client to the set
            } 
            if(sd > max_sd) { // Update the max socket
                max_sd = sd;
            }            
        }
        int activity = select(max_sd + 1, &clientSet, NULL, NULL, NULL);
        if(activity < 0) {
            continue; // No activity from clients
        }

        // determine if the listener has activity
        if(FD_ISSET(listener, &clientSet)) {
            // Accept Connection
            sd = accept(listener, NULL, NULL);
            if(sd == INVALID_SOCKET) { // Failure with the accept
                printf("accept failed: %d\n", WSAGetLastError());
                closesocket(listener);
                WSACleanup();
                return 1;
            }
            // Get client info
            getpeername(sd, (struct sockaddr*)&clientAddr, &clientAddrlen);
            printf("New connection from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

            // Send welcome
            sendRes = send(sd, welcomeStr, welcomeStrLen, 0);
            if(sendRes == SOCKET_ERROR) { // Failure with the send
                printf("send failed: %d\n", WSAGetLastError());
                shutdown(sd, SD_BOTH);
                closesocket(sd);
                continue;
            }

            // Add to the array 
            if(currentNoSockets >= MAX_CLIENTS) {
                printf("Full/n");
                shutdown(sd, SD_BOTH);
                closesocket(sd);
            } else {
                for(int i = 0; i < MAX_CLIENTS; i++) {
                    if(!clients[i]) {
                        clients[i] = sd;
                        currentNoSockets++;
                        printf("Added client %d\n", i);
                        break;
                    }
                }
            }
        }
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(!clients[i]) continue; // Skip empty slots
            sd = clients[i];
            if(FD_ISSET(sd, &clientSet)) { // Determine if the client has activity
                // Get message
                res = recv(sd, recvbuf, BUFLEN, 0);
                if(res > 0) { // Message received
                    // Print message
                    recvbuf[res] = '\0'; // Null terminate the string
                    printf("Message from client %d: %s\n", res, recvbuf);

                    if(!memcmp(recvbuf, "/quit", 5 * sizeof(char))) {
                        running = 0; // true
                        break;
                    }

                    // echo message
                    sendRes = send(sd, recvbuf, res, 0);
                    if(sendRes == SOCKET_ERROR) { // Failure with the send
                        printf("send failed: %d\n", WSAGetLastError());
                        shutdown(sd, SD_BOTH);
                        closesocket(sd);
                        clients[i] = 0; // Remove client from the array
                        currentNoSockets--; // Decrease the number of clients
                    }
                } else {
                    // Close Message
                    getpeername(sd, (struct sockaddr*)&clientAddr, &clientAddrlen);
                    printf("Client Disconnected at %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
                    shutdown(sd, SD_BOTH);
                    closesocket(sd);
                    clients[i] = 0; // Remove client from the array
                    currentNoSockets--; // Decrease the number of clients

                }
            }
        }
    }

    // CLEANUP - After using Winsock =============================
    
    // Disconnect all clients

    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i] > 0) {
            // active client
            sendRes = send(clients[i], quitStr, quitStrLen, 0);
            shutdown(clients[i], SD_BOTH);
            closesocket(clients[i]);
            clients[i] = 0; // Remove client from the array
            currentNoSockets--; // Decrease the number of clients
        }
    }
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