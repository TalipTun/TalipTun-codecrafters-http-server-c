#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


void getString(int pos, int len, int i, char string[], char substring[])
{

    while (i < len) {
        substring[i] = string[pos + i - 1];
        i++;
    }

    substring[i] = '\0';

    // Print the result
	printf("------\n");
    printf(substring);
    printf("\n");
	printf("------\n");
    return 0;
}

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);

	int server_fd, client_addr_len, client_fd;
	struct sockaddr_in client_addr;
	
	// Creating socket file descriptor
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
	
	client_addr_len = sizeof(client_addr);
	
	client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);

	if (client_fd < 0) {
        printf("Accept failed: %s\n", strerror(errno));
        return 1;
    }
	/*Received: GET / HTTP/1.1
	remote: [your_program] Host: localhost:4221
	expected code 200, got 404*/
	/*remote: [your_program] Received: GET /pear HTTP/1.1
	remote: [your_program] Host: localhost:4221
	remote: [your_program] 
	remote: [your_program] 
	remote: [tester::#IH0] Expected status code 404, got 200
	remote: [tester::#IH0] Test failed
	10th index*/


	char buffer[1024];
	int received_bytes = recv(client_fd, buffer, sizeof(buffer) , 0);
	printf("Received: %i\n", received_bytes);
	printf("Received: %s\n", buffer);
	char *a = strstr(buffer, "HTTP/1.1");
	char *b = strstr(buffer, "/echo/");
	printf("Index of first 'HTTP/1.1' in buffer: %ld\n", a - buffer);
	printf("Index of first '/echo/' in buffer: %ld\n", b - buffer);
	printf("aaaaaa %ld\n", (a -buffer) - (b - buffer) - 7);
	int lengthof_echo = (a - buffer) - (b - buffer) - 7;
	char substring[lengthof_echo + 1];
	getString(b - buffer + 7, lengthof_echo, 0, buffer, substring);
	printf("Received echo: %s\n", substring);
	printf("3\n");
	if (strstr(buffer, "GET /echo/")) {
		char *reply = ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %s\r\n\r\n", lengthof_echo);
		size_t len = strlen(reply) + strlen(substring);
		char *ret = (char*)malloc(len * sizeof(char) + 1);
		*ret = '\0';
		char *concatenated_reply = strcat(strcat(ret, reply) ,substring);
		printf("%s\n", strcat(strcat(ret, reply) ,substring));
		printf("-----\n");
		printf("%s\n", concatenated_reply);
		send(client_fd, concatenated_reply, strlen(concatenated_reply), 0); 
	} else {
		printf("2\n");
		char *reply = "HTTP/1.1 404 Not Found\r\n\r\n";
	    send(client_fd, reply, strlen(reply), 0);
	}
	printf("4\n");
	//cloose the client connection
	close(client_fd);
	close(server_fd);
	return 0;
}
