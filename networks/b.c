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

void gameprocess(char *matrix){
    //see if anyone won or not
    return;
}

int main()
{
    // Create socket
    int soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Define server address
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(6969),
        .sin_addr.s_addr = INADDR_ANY};

    // Bind the socket
    printf("Binding the socket to the address .... \n");
    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Error binding socket");
        close(soc);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    printf("Listening through the socket .... \n");
    if (listen(soc, 2) < 0)
    {
        perror("Error listening");
        close(soc);
        exit(EXIT_FAILURE);
    }

    // Initialize player count
    int players = 0;
    int soc_client_a = -1;
    int soc_client_b = -1;

    // Infinite loop to accept and handle clients
    while (players < 2)
    {
        printf("Waiting for clients to connect...\n");

        // Accept client A
        if (players == 0)
        {
            soc_client_a = accept(soc, NULL, NULL);
            if (soc_client_a < 0)
            {
                perror("Error accepting client A");
                close(soc);
                exit(EXIT_FAILURE);
            }
            players++;
            printf("Client A connected.\n");

            // Send prompt to Client A asking for their name
            char prompt_a[] = "Please enter your name (Client A): ";
            send(soc_client_a, prompt_a, strlen(prompt_a), 0);

            // Receive name from Client A
            char buffera[256] = {0};
            recv(soc_client_a, buffera, sizeof(buffera), 0);
            printf("Client A's name: %s\n", buffera);
        }

        // Accept client B
        if (players == 1)
        {
            soc_client_b = accept(soc, NULL, NULL);
            if (soc_client_b < 0)
            {
                perror("Error accepting client B");
                close(soc);
                exit(EXIT_FAILURE);
            }
            players++;
            printf("Client B connected.\n");

            // Send prompt to Client B asking for their name
            char prompt_b[] = "Please enter your name (Client B): ";
            send(soc_client_b, prompt_b, strlen(prompt_b), 0);

            // Receive name from Client B
            char bufferb[256] = {0};
            recv(soc_client_b, bufferb, sizeof(bufferb), 0);
            printf("Client B's name: %s\n", bufferb);
        }
    }

    // Both clients have connected, proceed with the game
    printf("Both clients connected, starting game...\n");

    // Example sending game prompt to both clients
    char game_prompt[] = "Please enter the number where you want to mark:\n ____________\n| 1 | 2 | 3 |\n ___________\n| 4 | 5 | 6 |\n ____________\n| 7 | 8 | 9 |\n ____________\n";
    char wait_prompt[] = "Please wait...\n";
    send(soc_client_a, game_prompt, strlen(game_prompt), 0);
    send(soc_client_b, wait_prompt, strlen(wait_prompt), 0);

    // Add more logic to handle turns, receive moves, and update the game state
    while (1)
    {
        int matrix[9]={0};

        char buffera[1] = {0};
        recv(soc_client_a, buffera, sizeof(buffera), 0);

        matrix[(int)buffera[0]]++;
        game_process(*matrix);

        //mark the matrix with the number they choose x or o depending on player and then send the updated board to b while a waits and then again repeat
        

    }
    

    // Close client sockets after the game ends
    close(soc_client_a);
    close(soc_client_b);

    // Close the server socket
    close(soc);

    return 0;
}
