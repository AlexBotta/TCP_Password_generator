#ifndef PROTOCOL_H
#define PROTOCOL_H

// Default IP address for the server
#define DEFAULT_IP "127.0.0.1"

// Default port for the server
#define DEFAULT_PORT 12345

// Maximum allowed length for a generated password
#define MAX_PASSWORD_LENGTH 32

// Minimum allowed length for a generated password
#define MIN_PASSWORD_LENGTH 6

// Command definitions used by the client to request specific password types

// Command to quit the connection with the server
#define CMD_QUIT "q"

// Command to generate a numeric password (only digits)
#define CMD_NUMERIC "n"

// Command to generate an alphabetic password (only lowercase letters)
#define CMD_ALPHA "a"

// Command to generate a mixed password (letters and numbers)
#define CMD_MIXED "m"

// Command to generate a secure password (letters, numbers, and special symbols)
#define CMD_SECURE "s"

// Function prototypes for password generation

//Generates a numeric password containing only digits.
char* generate_numeric(int length);

//Generates an alphabetic password containing only lowercase letters.
char* generate_alpha(int length);

//Generates a mixed password containing letters (lowercase) and numbers.
char* generate_mixed(int length);

//Generates a secure password containing uppercase letters, lowercase letters, numbers, and special symbols.
char* generate_secure(int length);

#endif // End of protocol.h
