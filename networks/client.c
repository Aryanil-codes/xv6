#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6969
#define BUFFER_SIZE 256

void display_board(const char *board) {
    printf("\nCurrent board state:\n%s\n", board);
}

int main(int argc, char *argv[]) {
    int soc;
    char *ip = "127.0.0.1";
    if (argc > 1) {
        ip = argv[1];  // Assign the first argument to ip
        printf("The IP string is: %s\n", ip);
    } else {
        printf("No argument provided so we took localhost!\n");
    }
    
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    char player_name[BUFFER_SIZE] = {0};
    
    // Create socket
    if ((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // Convert address to binary and set server address
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        printf("\nInvalid address or address not supported\n");
        return -1;
    }

    // Connect to the server
    if (connect(soc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("\nConnection to the server failed\n");
        return -1;
    }

    // Receive prompt to enter name
    read(soc, buffer, BUFFER_SIZE);
    printf("%s", buffer);

    // Get player's name
    fgets(player_name, sizeof(player_name), stdin);
    player_name[strcspn(player_name, "\n")] = '\0'; // Remove newline character
    send(soc, player_name, strlen(player_name), 0);

    // Game loop
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive the board or waiting message
        int bytes_read = read(soc, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            printf("Server closed the connection.\n");
            break;
        }

        // Check if it's a game over message (win/lose/draw)
        if (strstr(buffer, "Game over") != NULL) {
            printf("%s\n", buffer);
            break;  // Exit after the game ends
        }

        // Display the board or any message received
        display_board(buffer);

        // If the prompt is asking for a move, input the move
        if (strstr(buffer, "Please wait...") == NULL) {
            printf("Enter your move (1-9): ");
            char move[2];
            fgets(move, sizeof(move), stdin);
            send(soc, move, strlen(move), 0);
        }
    }

    // Close the socket
    close(soc);
    return 0;
}
