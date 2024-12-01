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

	char buffer[1024];
	int received_bytes = recv(client_fd, buffer, sizeof(buffer) , 0);
	printf("Received from client: %s\n", buffer);
	char *path = strtok(buffer, " ");
  	path = strtok(NULL, " ");
	if (strncmp(path, "/echo/", 6) == 0) {
		char *echo_string = path + 6;
		char response[1024];
		sprintf(response,
            "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
            "%ld\r\n\r\n%s",
            strlen(echo_string), echo_string);
		send(client_fd, response, strlen(response), 0); 
	} else if (strcmp(path, "/") == 0) {
		const char *response = "HTTP/1.1 200 OK\r\n\r\n";
		send(client_fd, response, strlen(response), 0);
	} else if (strncmp(path, "/user-agent", 11) == 0) {
		for (int i = 0; i < 3; i++) { path = strtok(NULL, " "); }
		char response[1024]; 
		sprintf(response,
            "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
            "%ld\r\n\r\n%s",
            strlen(path), path);
		printf("Response: %s\n", response);
		send(client_fd, response, strlen(path), 0);
	} else {
		char *reply = "HTTP/1.1 404 Not Found\r\n\r\n";
	    send(client_fd, reply, strlen(reply), 0);
	}

	//cloose the client connection
	close(client_fd);
	close(server_fd);
	return 0;
}
