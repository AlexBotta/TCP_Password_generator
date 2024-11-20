#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "protocol.h"

#pragma comment(lib, "Ws2_32.lib") // Links the Winsock library required for socket programming on Windows

int main() {
    WSADATA wsa_data;                // Structure to hold Windows Sockets initialization data
    SOCKET client_socket;            // Socket descriptor for the client
    struct sockaddr_in server_addr;  // Structure to hold server address information
    char buffer[1024];               // Buffer for sending and receiving data

    // Initialize Winsock library
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("WSAStartup failed.\n");
        return 1; // Exit with an error code if initialization fails
    }

    // Create a socket for the client
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup(); // Clean up Winsock resources
        return 1; // Exit with an error code if socket creation fails
    }

    // Configure the server address for connection
    server_addr.sin_family = AF_INET;            // Internet address family (IPv4)
    server_addr.sin_addr.s_addr = inet_addr(DEFAULT_IP); // Set the IP address (from protocol.h)
    server_addr.sin_port = htons(DEFAULT_PORT);  // Set the port (from protocol.h)

    // Attempt to connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed.\n");
        closesocket(client_socket); // Close the socket on failure
        WSACleanup(); // Clean up Winsock resources
        return 1; // Exit with an error code if connection fails
    }

    printf("Connected to server.\n"); // Notify the user of a successful connection

    // Loop to handle user commands and interact with the server
    while (1) {
        // Print command instructions for the user
        printf("n: numeric password (only digits)\n"
               "a: alphabetical password (only lowercase letters)\n"
               "m: mixed password (lowercase and numerals)\n"
               "s: Secure password (uppercase, lowercase, numbers, and symbols)\n");
        printf("Enter command (e.g., n 8, a 12, q): ");

        // Read the user's input command
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline character from input

        // Send the user's command to the server
        send(client_socket, buffer, strlen(buffer), 0);

        // If the user sends the quit command ('q'), terminate the connection
        if (buffer[0] == 'q') {
            printf("Disconnected from server.\n");
            break;
        }

        // Clear the buffer and wait for the server's response
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        // Display the generated password received from the server
        printf("\nGenerated password: %s\n\n", buffer);
    }

    // Close the socket and clean up Winsock resources
    closesocket(client_socket);
    WSACleanup();
    return 0; // Exit the program successfully
}
