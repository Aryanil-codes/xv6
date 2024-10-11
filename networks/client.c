#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdbool.h>

int result = -1;  // 1: Player A wins, 2: Player B wins, 3: Draw, -1: Ongoing game

char buffera[256] = {0};
char bufferb[256] = {0};

void display_board(int *matrix, char *board) {
    for (int i = 0; i < 9; i++) {
        if (matrix[i] == 1) {
            board[i * 2] = 'X';
        } else if (matrix[i] == -1) {
            board[i * 2] = 'O';
        } else {
            board[i * 2] = '1' + i;
        }
    }
}

void gameprocess(int *matrix) {
    // Check rows, columns, and diagonals
    for (int i = 0; i < 3; i++) {
        if (matrix[i*3] == matrix[i*3+1] && matrix[i*3+1] == matrix[i*3+2] && matrix[i*3] != 0) {
            result = (matrix[i*3] == 1) ? 1 : 2;
            return;
        }
        if (matrix[i] == matrix[i+3] && matrix[i+3] == matrix[i+6] && matrix[i] != 0) {
            result = (matrix[i] == 1) ? 1 : 2;
            return;
        }
    }
    if ((matrix[0] == matrix[4] && matrix[4] == matrix[8] && matrix[0] != 0) ||
        (matrix[2] == matrix[4] && matrix[4] == matrix[6] && matrix[2] != 0)) {
        result = (matrix[4] == 1) ? 1 : 2;
        return;
    }

    // Check for draw
    bool draw = true;
    for (int i = 0; i < 9; i++) {
        if (matrix[i] == 0) {
            draw = false;
            break;
        }
    }
    if (draw) {
        result = 3;
    }
}

void reset_game(int *matrix, char *board) {
    memset(matrix, 0, 9 * sizeof(int));
    strcpy(board, "1 2 3\n4 5 6\n7 8 9\n");
    result = -1;
}

void send_board_to_both(int soc_client_a, int soc_client_b, char *board, char *turn_msg, char *wait_msg) {
    char board_with_msg_a[256], board_with_msg_b[256];
    snprintf(board_with_msg_a, sizeof(board_with_msg_a), "%s\n%s", board, turn_msg);
    snprintf(board_with_msg_b, sizeof(board_with_msg_b), "%s\n%s", board, wait_msg);
    
    send(soc_client_a, board_with_msg_a, strlen(board_with_msg_a), 0);
    send(soc_client_b, board_with_msg_b, strlen(board_with_msg_b), 0);
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
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error binding socket");
        close(soc);
        exit(EXIT_FAILURE);
    }

    if (listen(soc, 2) < 0) {
        perror("Error listening");
        close(soc);
        exit(EXIT_FAILURE);
    }

    int players = 0;
    int soc_client_a = -1;
    int soc_client_b = -1;

    while (players < 2) {
        int *client_socket = (players == 0) ? &soc_client_a : &soc_client_b;
        *client_socket = accept(soc, NULL, NULL);
        if (*client_socket < 0) {
            perror("Error accepting client");
            close(soc);
            exit(EXIT_FAILURE);
        }
        players++;

        char prompt[] = "Please enter your name: ";
        send(*client_socket, prompt, strlen(prompt), 0);

        char *buffer = (players == 1) ? buffera : bufferb;
        recv(*client_socket, buffer, sizeof(buffera) - 1, 0);
        buffer[strcspn(buffer, "\n")] = 0;
        printf("Client %c's name: %s\n", (players == 1) ? 'A' : 'B', buffer);
    }

    printf("Both clients connected, starting game...\n");

    char board[20] = "1 2 3\n4 5 6\n7 8 9\n";
    char turn_msg[] = "Your turn. Enter your move (1-9): ";
    char wait_msg[] = "Please wait for your turn...";
    int matrix[9] = {0};

    while (1) {
        if (result > 0) {
            char msg_a[256], msg_b[256];
            if (result == 1 || result == 2) {
                sprintf(msg_a, "%s\n%s wins!\n", board, (result == 1) ? "You" : bufferb);
                sprintf(msg_b, "%s\n%s wins!\n", board, (result == 2) ? "You" : buffera);
            } else {
                sprintf(msg_a, "%s\nIt's a draw!\n", board);
                sprintf(msg_b, "%s\nIt's a draw!\n", board);
            }
            strcat(msg_a, "Do you want to play again? (y/n)\n");
            strcat(msg_b, "Do you want to play again? (y/n)\n");
            send(soc_client_a, msg_a, strlen(msg_a), 0);
            send(soc_client_b, msg_b, strlen(msg_b), 0);

            char response_a, response_b;
            recv(soc_client_a, &response_a, 1, 0);
            recv(soc_client_b, &response_b, 1, 0);

            if (response_a == 'y' && response_b == 'y') {
                reset_game(matrix, board);
            } else {
                char end_msg[] = "Game over. Thanks for playing!\n";
                send(soc_client_a, end_msg, strlen(end_msg), 0);
                send(soc_client_b, end_msg, strlen(end_msg), 0);
                break;
            }
            continue;
        }

        // Player A's turn
        send_board_to_both(soc_client_a, soc_client_b, board, turn_msg, wait_msg);

        char move_a;
        recv(soc_client_a, &move_a, 1, 0);
        int pos_a = move_a - '1';
        if (pos_a >= 0 && pos_a < 9 && matrix[pos_a] == 0) {
            matrix[pos_a] = 1;
            display_board(matrix, board);
            gameprocess(matrix);
        }

        if (result > 0) continue;

        // Player B's turn
        send_board_to_both(soc_client_b, soc_client_a, board, turn_msg, wait_msg);

        char move_b;
        recv(soc_client_b, &move_b, 1, 0);
        int pos_b = move_b - '1';
        if (pos_b >= 0 && pos_b < 9 && matrix[pos_b] == 0) {
            matrix[pos_b] = -1;
            display_board(matrix, board);
            gameprocess(matrix);
        }
    }

    close(soc_client_a);
    close(soc_client_b);
    close(soc);
    return 0;
}