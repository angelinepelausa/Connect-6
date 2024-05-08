#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include "functions.c"

int main(int argc, char *argv[]){
    char c;
    char player_turn = 'O'; //first turn starts with 'O'
   
    char table[9][9];
    int server_sock, client_sock, port_no, client_size, n;
    char buffer[256];
    bool ismain_loop = 1;
    struct sockaddr_in server_addr, client_addr;
    if (argc < 2) {
        printf("Usage: %s port_no", argv[0]);
        exit(1);
    }

    printf("PC2 starting ...\n");
    // Create a socket for incoming connections
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) 
       die_with_error("Error: socket() Failed.");
       
    // Bind socket to a port
    bzero((char *) &server_addr, sizeof(server_addr));
    port_no = atoi(argv[1]);
    server_addr.sin_family = AF_INET; // Internet address 
    server_addr.sin_addr.s_addr = INADDR_ANY; // Any incoming interface
    server_addr.sin_port = htons(port_no); // Local port
    
    if (bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
       die_with_error("Error: bind() Failed.");
       
    // Mark the socket so it will listen for incoming connections
    listen(server_sock, 5);
    printf("PC2 listening to port %d ...\n", port_no);
    
    printf("Waiting for connection(s) ...\n");
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            table[i][j] = ' ';
        }
    }
    while(ismain_loop == 1){
    display(table);
    
    // Accept new connection
    client_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_size);
    if (client_sock < 0) 
        die_with_error("Error: accept() Failed.");

    printf("PC1 succesfully connected ...\n");    
    // Communicate  
    while(1){  
    bzero(buffer, 256);
    n = recv(client_sock, buffer, 255, 0);
    if (n < 0) die_with_error("Error: recv() Failed.");
    printf("Message received from PC1 : %s", buffer);
     
    option(buffer, 'O', table);
    display(table);
    if (Winner(table, 'O')) {
        printf("Player 1 wins!\n");
        ismain_loop = 0;
        break;
    }

    bzero(buffer, 256);
    do {
     	printf("Enter column : ");
    	fgets(buffer, 255, stdin);
    } while(tolower(buffer[0]) != 'a' && tolower(buffer[0]) != 'b' &&
    	    tolower(buffer[0]) != 'c' && tolower(buffer[0]) != 'd' &&
    	    tolower(buffer[0]) != 'e' && tolower(buffer[0]) != 'f' &&
    	    tolower(buffer[0]) != 'g' && tolower(buffer[0]) != 'h');
    option(buffer, 'X', table);
    display(table);
    n = send(client_sock, buffer, strlen(buffer), 0);
    if (Winner(table, 'X')) {
        printf("Player 2 wins!\n");
        ismain_loop = 0;
        break;
    }
    
    
    if (n < 0) die_with_error("Error: send() Failed.");
    }
    }
    printf("Closing connection ...\n");
    close(client_sock);
    close(server_sock);
    
    return 0; 
}