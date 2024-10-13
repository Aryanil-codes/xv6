#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>    // close
#include <arpa/inet.h> // for inet_ntoa
#include <errno.h>
#include <sys/wait.h>
#include <stdbool.h>

int result = -1;

char buffera[256] = {0};
char bufferb[256] = {0};

void display_board(int *matrix, char *board) {
    // Fill the board with current state of the game using 'X', 'O' or numbers for empty spaces
    for (int i = 0; i < 9; i++) {
        if (matrix[i] == 1) {
            board[i * 2] = 'X'; // Player A move
        } else if (matrix[i] == -1) {
            board[i * 2] = 'O'; // Player B move
        } else {
            board[i * 2] = '1' + i; // Empty spot, show the number
        }
    }
}

void gameprocess(int *matrix) {
    // Check for any win condition (rows, columns, or diagonals)
    for (int i = 0; i < 3; i++) {
        if (matrix[i * 3] == matrix[i * 3 + 1] && matrix[i * 3 + 1] == matrix[i * 3 + 2] && matrix[i * 3] != 0) {
            printf("Player %d wins!\n", matrix[i * 3] == 1 ? 1 : 2);
            // result = (matrix[i * 3] == 1 ? 1 : 2);
            if(matrix[i*3]==1){
                result =1;
            }
            else{
                result =2;
            }
            return;
        }
    }

    for (int i = 0; i < 3; i++) {
        if (matrix[i] == matrix[i + 3] && matrix[i + 3] == matrix[i + 6] && matrix[i] != 0) {
            printf("Player %d wins!\n", matrix[i] == 1 ? 1 : 2);
            // result = matrix[i] == 1 ? 1 : 2;
            if(matrix[i]==1){
                result =1;
            }
            else{
                result =2;
            }
            return;
        }
    }

    if ((matrix[0] == matrix[4] && matrix[4] == matrix[8]) || 
        (matrix[2] == matrix[4] && matrix[4] == matrix[6])) {
        printf("Player %d wins!\n", matrix[4] == 1 ? 1 : 2);
        // result = matrix[4] == 1 ? 1 : 2;
        if(matrix[4]==1){
                result =1;
            }
            else{
                result =2;
            }
        return;
    }

    bool draw = true;
    for (int i = 0; i < 9; i++) {
        if (matrix[i] == 0) {
            draw = false;
            break;
        }
    }
    if (draw) {
        printf("It's a draw!\n");
        result = 3;
    }
    return;
}

void reset_game(int *matrix, char *board) {
    memset(matrix, 0, 9 * sizeof(int)); // Reset the game matrix
    strcpy(board, "1 2 3\n4 5 6\n7 8 9\n"); // Reset the board display
    result = -1; // Reset result for a new game
}

int main() {
    int soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(6969),
        .sin_addr.s_addr = INADDR_ANY};

    printf("Binding the socket to the address .... \n");
    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error binding socket");
        close(soc);
        exit(EXIT_FAILURE);
    }

    printf("Listening through the socket .... \n");
    if (listen(soc, 2) < 0) {
        perror("Error listening");
        close(soc);
        exit(EXIT_FAILURE);
    }

    int players = 0;
    int soc_client_a = -1;
    int soc_client_b = -1;

    while (players < 2) {
        printf("Waiting for clients to connect...\n");

        if (players == 0) {
            soc_client_a = accept(soc, NULL, NULL);
            if (soc_client_a < 0) {
                perror("Error accepting client A");
                close(soc);
                exit(EXIT_FAILURE);
            }
            players++;
            printf("Client A connected.\n");

            char prompt_a[] = "Please enter your name (Client A): ";
            send(soc_client_a, prompt_a, strlen(prompt_a), 0);

            recv(soc_client_a, buffera, sizeof(buffera), 0);
            printf("Client A's name: %s\n", buffera);
        }

        if (players == 1) {
            soc_client_b = accept(soc, NULL, NULL);
            if (soc_client_b < 0) {
                perror("Error accepting client B");
                close(soc);
                exit(EXIT_FAILURE);
            }
            players++;
            printf("Client B connected.\n");

            char prompt_b[] = "Please enter your name (Client B): ";
            send(soc_client_b, prompt_b, strlen(prompt_b), 0);

            recv(soc_client_b, bufferb, sizeof(bufferb), 0);
            printf("Client B's name: %s\n", bufferb);
        }
    }

    printf("Both clients connected, starting game...\n");

    char board[20]; // Adjust size for proper string manipulation
    int matrix[9] = {0}; // Game state matrix

    reset_game(matrix, board); // Initialize the game state

    while (1) {
        if (result > 0) {
            if (result == 1) {
                // Player A wins
                char win_msg_a[] = "You win, Player A!\n";
                char lose_msg_b[] = "Player B wins!\n";
                send(soc_client_a, win_msg_a, strlen(win_msg_a), 0);
                send(soc_client_b, lose_msg_b, strlen(lose_msg_b), 0);
            } else if (result == 2) {
                // Player B wins
                char win_msg_b[] = "You win, Player B!\n";
                char lose_msg_a[] = "Player A wins!\n";
                send(soc_client_b, win_msg_b, strlen(win_msg_b), 0);
                send(soc_client_a, lose_msg_a, strlen(lose_msg_a), 0);
            } else if (result == 3) {
                // Draw
                char draw_msg[] = "It's a draw!\nDo you want to play again? (y/n)\n";
                send(soc_client_a, draw_msg, strlen(draw_msg), 0);
                send(soc_client_b, draw_msg, strlen(draw_msg), 0);

                // Receive responses from both players
                char response_a[1], response_b[1];
                recv(soc_client_a, response_a, 1, 0);
                recv(soc_client_b, response_b, 1, 0);

                if (response_a[0] == 'y' && response_b[0] == 'y') {
                    // Reset the game board and continue
                    reset_game(matrix, board);
                } else {
                    char end_msg[] = "Game over. Thanks for playing!\n";
                    send(soc_client_a, end_msg, strlen(end_msg), 0);
                    send(soc_client_b, end_msg, strlen(end_msg), 0);
                    break; // Exit the loop to end the game
                }
            }
            // If the game has ended, skip further processing
            continue;
        }

        // Send board to Client A
        send(soc_client_a, board, strlen(board), 0);
        send(soc_client_b, "Please wait...\n", strlen("Please wait...\n"), 0);

        // Receive move from Client A
        char move_a[1] = {0};
        recv(soc_client_a, move_a, sizeof(move_a), 0);
        int pos_a = move_a[0] - '1'; // Convert input to index
        if (pos_a >= 0 && pos_a < 9 && matrix[pos_a] == 0) {
            matrix[pos_a] = 1; // Mark as Player A's move
        }

        // Update and display the board
        display_board(matrix, board);
        gameprocess(matrix);

        if (result > 0) continue; // Check if the game ended after A's move

        // Send updated board to Client B
        send(soc_client_b, board, strlen(board), 0);
        send(soc_client_a, "Please wait...\n", strlen("Please wait...\n"), 0);

        // Receive move from Client B
        char move_b[1] = {0};
        recv(soc_client_b, move_b, sizeof(move_b), 0);
        int pos_b = move_b[0] - '1'; // Convert input to index
        if (pos_b >= 0 && pos_b < 9 && matrix[pos_b] == 0) {
            matrix[pos_b] = -1; // Mark as Player B's move
        }

        // Update and display the board
        display_board(matrix, board);
        gameprocess(matrix);
    }

    close(soc_client_a);
    close(soc_client_b);
    close(soc);
    return 0;
}
