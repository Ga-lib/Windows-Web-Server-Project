#include <stdio.h>
#include <winsock2.h>
#include <string.h> // For using memset and strcpy

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

void handle_client(SOCKET clientSocket) {
    char buffer[1024];
    int received_bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (received_bytes < 1) {
        printf("Connection closed or error occurred.\n");
    } else {
        // Print out the request for debugging
        printf("Received (%d bytes): %.*s\n", received_bytes, received_bytes, buffer);

        // Send a simple HTTP response
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, World!";
        send(clientSocket, response, strlen(response), 0);
    }

    // Close the client socket
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in server, client;
    int c;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize WSADATA: %d\n", WSAGetLastError());
        return 1;
    }

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen to incoming connections
    if (listen(serverSocket, 3) == SOCKET_ERROR) {
        printf("Listen failed with error code: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Accept incoming connections
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    while ((clientSocket = accept(serverSocket, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) {
        printf("Connection accepted\n");
        handle_client(clientSocket);
    }

    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed with error code: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}


// open cmd and enter these:
// cd C:\Users\ASRock\Documents\C program project\WinsockServer
//gcc -o WinsockServer server.c -lws2_32
// .\WinsockServer
// enter browser, curl http://localhost:8080
// will show hello world