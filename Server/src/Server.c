#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <winsock2.h>
#include "protocol.h"

#pragma comment(lib, "Ws2_32.lib") // Links Winsock library required for Windows networking

// Function to generate numeric passwords
char* generate_numeric(int length) {
	 // Allocate memory for the password
    char* password = (char*)malloc(length + 1);
    const char* digits = "0123456789"; // Allowed characters
    for (int i = 0; i < length; i++) {
    	// Randomly select a digit
        password[i] = digits[rand() % strlen(digits)];
    }
    password[length] = '\0'; // Null-terminate the string
    return password;
}

// Function to generate alphabetic passwords (lowercase only)
char* generate_alpha(int length) {
    char* password = (char*)malloc(length + 1);
    const char* letters = "abcdefghijklmnopqrstuvwxyz"; // Allowed characters
    for (int i = 0; i < length; i++) {
        password[i] = letters[rand() % strlen(letters)];
    }
    password[length] = '\0'; // Null-terminate the string
    return password;
}

// Function to generate mixed passwords (letters and numbers)
char* generate_mixed(int length) {
    char* password = (char*)malloc(length + 1);
    const char* mixed = "abcdefghijklmnopqrstuvwxyz0123456789"; // Allowed characters
    for (int i = 0; i < length; i++) {
        password[i] = mixed[rand() % strlen(mixed)];
    }
    password[length] = '\0'; // Null-terminate the string
    return password;
}

// Function to generate secure passwords (letters, numbers, and symbols)
char* generate_secure(int length) {
    char* password = (char*)malloc(length + 1);
    const char* secure = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()"; // Allowed characters
    for (int i = 0; i < length; i++) {
        password[i] = secure[rand() % strlen(secure)];
    }
    password[length] = '\0'; // Null-terminate the string
    return password;
}

int main() {
    WSADATA wsa_data;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024]; // Buffer to store client data
    int addr_len = sizeof(client_addr);

    srand((unsigned int)time(NULL)); // Seed the random number generator

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("WSAStartup failed.\n");
        return 1; // Exit if Winsock initialization fails
    }

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup(); // Cleanup Winsock on failure
        return 1;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(DEFAULT_IP); // Use default IP address
    server_addr.sin_port = htons(DEFAULT_PORT);			 // Use default port

    // Bind the socket to the specified IP and port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(server_socket); // Close the socket on failure
        WSACleanup();				// Cleanup Winsock
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed.\n");
        closesocket(server_socket); // Close the socket on failure
        WSACleanup();				// Cleanup Winsock
        return 1;
    }

    printf("Server listening on %s:%d\n", DEFAULT_IP, DEFAULT_PORT);

    // Main loop to handle client connections
    while (1) {
    	// Accept a new client connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed.\n");
            continue; // Skip to the next iteration on failure
        }

        printf("New connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), // Convert client IP to a string
               ntohs(client_addr.sin_port));	// Convert client port to host byte order

        // Handle client requests
        while (1) {
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer
            int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received <= 0) {
                printf("Client disconnected.\n");
                break; // Exit the loop if the client disconnects
            }

            buffer[bytes_received] = '\0'; // Null-terminate the received data

            // Parse client command
            char command = buffer[0];		// First character is the command
            int length = atoi(&buffer[2]);	// Extract the length from the input

            char* password = NULL; // Pointer to store the generated password

            // Validate password length
            if (length < MIN_PASSWORD_LENGTH || length > MAX_PASSWORD_LENGTH) {
                const char* error_msg = "Invalid length.\n";
                send(client_socket, error_msg, strlen(error_msg), 0); // Send error to client
                continue;
            }
            // Generate password based on the command
            switch (command) {
                case 'n': password = generate_numeric(length); break;	// Numeric password
                case 'a': password = generate_alpha(length); break;		// Alphabetic password
                case 'm': password = generate_mixed(length); break;		// Mixed password
                case 's': password = generate_secure(length); break;	// Secure password
                case 'q': closesocket(client_socket) /* Close client socket */; free(password) /* Free allocated memory */; goto end_client; // Exit handling this client
                default:
                    send(client_socket, "Invalid command.\n", 18, 0); // Invalid command
                    continue;
            }
            // Send the generated password to the client
            send(client_socket, password, strlen(password), 0);
            free(password); // Free the allocated memory for the password
        }
    end_client:
        closesocket(client_socket); // Ensure the client socket is closed
    }

    // Cleanup
    closesocket(server_socket); // Close the server socket
    WSACleanup();				// Cleanup Winsock
    return 0;
}
