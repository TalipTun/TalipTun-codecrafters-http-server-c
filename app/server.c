#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main() {
	printf("Starting server...\n");
	// Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	int server_fd, client_addr_len, client_fd;
	struct sockaddr_in client_addr;
	
	// Creating socket file descriptor
	printf("Connecting");
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}
	
	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors

	// Set socket options to reuse address and port
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
		return 1;
	}
	
	struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
									 .sin_port = htons(4221),
									 .sin_addr = { htonl(INADDR_ANY) },
									};
	
	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}
	
	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0) {
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}
	
	printf("Waiting for a client to connect...\n");
	client_addr_len = sizeof(client_addr);
	
	client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);

	if (client_fd < 0) {
        printf("Accept failed: %s\n", strerror(errno));
        return 1;
    }
	printf("Client connected\n");

	char response_200[] =
        "HTTP/1.1 200 OK\r\n"       // HTTP version and status code
        "Content-Length: 13\r\n"    // Length of the body
        "Content-Type: text/plain\r\n" // Content type
        "\r\n"                      // Blank line to separate headers and body
        "Hello, World!";            // Response body

	char response_404[] = 
		"HTTP/1.1 404 Not Found\r\n"   // HTTP version and status code
        "Content-Type: text/plain\r\n" // Content type
        "Content-Length: 15\r\n"       // Content length
        "\r\n"                         // Blank line to separate headers and body
        "Page Not Found";              // Response body
/*
	if (send(client_fd, response_200, strlen(response_200), 0) < 0) {
		printf("Send failed: %s \n", strerror(errno));
        return 1;
	} else {
		printf("Response sent successfully\n");
	}
*/	printf("1\n");
	char request[] = "";
	int response = recv(client_fd, request, sizeof(request), 0);
	if (strstr("200" , request) || response < 0) {
		printf("2\n");
		printf("%s\n",request);
		send(client_fd, response_404, strlen(response_404), 0);
    } else { 
		printf("3\n");
		printf("%s\n",request);
		send(client_fd, response_200, strlen(response_200), 0);
	}
	printf("4\n");

	//cloose the client connection
	close(client_fd);

	printf("Closing the server\n");
	close(server_fd);
	printf("Server closedd\n");
	return 0;
}
