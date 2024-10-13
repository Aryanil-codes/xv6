#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> //close


main(){
    int soc = socket(AF_INET, SOCK_STREAM,0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(4242),
        .sin_addr = 0
    };

    printf("binding the socket to the address .... \n");
    bind(soc, &addr, sizeof(addr));

    printf("listening through the socket .... \n");
    listen(soc,2);

    while (1)
    {
        

        int fake_pid = fork();

        if(fake_pid==0){
            printf("accepting the client .... \n");
            int soc_client_a = accept(soc, 0 , 0);
            // char buffera[256] = {0};
            // printf("recieving the data from the client .... \n");
            // recv(soc_client_a, buffera , 256 ,0);
            // printf("%s\n",buffera);

            char prompt_a[] = "Please enter your name (Client A): ";
            send(soc_client_a, prompt_a, strlen(prompt_a), 0);


            char buffera[256] = {0};
            recv(soc_client_a, buffera, sizeof(buffera), 0);
            printf("Client A's name: %s\n", buffera);

            close(soc_client_a); // Close the client socket after processing
            exit(0);
        }
        else{
            printf("accepting the client .... \n");
            int soc_client_b = accept(soc, 0 , 0);
            // char bufferb[256] = {0};
            // printf("recieving the data from the client .... \n");
            // recv(soc_client_b, bufferb , 256 ,0);
            // printf("%s\n",bufferb);

            char prompt_b[] = "Please enter your name (Client B): ";
            send(soc_client_b, prompt_b, strlen(prompt_b), 0);

            // Receive name from Client B
             
            printf("Client B's name: %s\n", bufferb);

            close(soc_client_b);
        }
    }
    

    
    
    

    return 0;
}


// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netinet/ip.h>
// #include <string.h>
// #include <errno.h>
// #include <unistd.h>

// int main() {
// 	// Disable output buffering
// 	setbuf(stdout, NULL);
//  	setbuf(stderr, NULL);

// 	// You can use print statements as follows for debugging, they'll be visible when running tests.
// 	printf("Logs from your program will appear here!\n");

// 	// Uncomment this block to pass the first stage
	
// 	int server_fd, client_addr_len;
// 	struct sockaddr_in client_addr;
	
// 	server_fd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (server_fd == -1) {
// 		printf("Socket creation failed: %s...\n", strerror(errno));
// 		return 1;
// 	}
	
// 	// Since the tester restarts your program quite often, setting SO_REUSEADDR
// 	// ensures that we don't run into 'Address already in use' errors
// 	int reuse = 1;
// 	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
// 		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
// 		return 1;
// 	}
	
// 	struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
// 									 .sin_port = htons(4221),
// 									 .sin_addr = { htonl(INADDR_ANY) }, 
// 									};
	
// 	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
// 		printf("Bind failed: %s \n", strerror(errno));
// 		return 1;
// 	}
	
// 	int connection_backlog = 5;
// 	if (listen(server_fd, connection_backlog) != 0) {
// 		printf("Listen failed: %s \n", strerror(errno));
// 		return 1;
// 	}
	
// 	printf("Waiting for a client to connect...\n");
// 	client_addr_len = sizeof(client_addr);
	
// 	// accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
// 	int client_fd = accept(server_fd,(struct sockaddr *) &client_addr, &client_addr_len); //fd from the client

// 	char readbuffer[1024];
// 	char path[512];
// 	int bytesreceived = recv(client_fd, readbuffer, sizeof(readbuffer),0);

// 	char *reqpath = strtok(readbuffer," ");
// 	reqpath = strtok(NULL," ");

// 	int bytessent;

// 	if (strcmp(reqpath, "/")==0)
// 	{
// 		char *res = "HTTP/1.1 200 OK\r\n\r\n";
// 		bytessent = send(client_fd,res,strlen(res),0);
// 	}
// 	else{
// 		char *res = "HTTP/1.1 404 Not Found\r\n\r\n";
// 		bytessent = send(client_fd,res,strlen(res),0);
// 	}

// 	if (bytessent<0)
// 	{
// 		printf("bytessent failed\n");
// 		return 1;
// 	}
	
	

// 	printf("Client connected\n");

// 	// char* reply = "HTTP/1.1 200 OK\r\n\r\n";

// 	// int byte = send(fd,reply,strlen(reply),0);

// 	close(client_fd);


	
// 	close(server_fd);

// 	return 0;
// }