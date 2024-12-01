#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE 4098


int main(int argc, char **argv) {
	char *directory = NULL;
	if (argc >= 2 && (strncmp(argv[1], "--directory", 11) == 0)) {
		directory = argv[2];
	}
	// Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);

	int server_fd, client_addr_len, client_fd;
	struct sockaddr_in client_addr;
	printf("Server is running...\n");
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

    printf("Server is listening...\n");
	
	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0) {
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}

	printf("Waiting for new connections...\n");

	client_addr_len = sizeof(client_addr);
	// here
	fork();
	fork();
	pid_t  p = fork();
    if(p<0){
      perror("fork fail");
      exit(1);
    }

	printf("Client connected...\n");

	client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);

	if (client_fd < 0) {
        printf("Acceptt failed: %s\n", strerror(errno));
        return 1;
    }

	printf("Received client request...\n");

	char buffer[1024];
	int received_bytes = recv(client_fd, buffer, sizeof(buffer) , 0);
	printf("Received: %s\n", buffer);
	char *path = strtok(buffer, " ");
	printf("--------");
	printf("%s\n", buffer);
	printf("--------");
  	path = strtok(NULL, " ");
	printf("--------");
	printf("%s\n", path);
	printf("--------");
	if (strncmp(path, "/echo/", 6) == 0) {
		printf("2\n");
		char *echo_string = path + 6;
		char response[1024];
		sprintf(response,
            "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
            "%ld\r\n\r\n%s",
            strlen(echo_string), echo_string);
		send(client_fd, response, strlen(response), 0); 
	} else if (strcmp(path, "/") == 0) {
		printf("3\n");
		const char *response = "HTTP/1.1 200 OK\r\n\r\n";
		send(client_fd, response, strlen(response), 0);
	} else if (strncmp(path, "/user-agent", 11) == 0) {
		printf("4\n");
		for (int i = 0; i < 3; i++) { path = strtok(NULL, " "); }
		char response[1024];
		printf("%s\n", path);
		printf("%i\n",strlen(path));
		sprintf(response,
            "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
            "%ld\r\n\r\n%s",
            strlen(path) - 4, path);
		send(client_fd, response, strlen(response), 0);
	} else if (strncmp(path, "/files", 6) == 0) {
		printf("33333\n");
		char response[1024];
		char *file = strchr(path + 1, '/');
		if (file != NULL) {
			printf("444444\n");
			char *filepath = strcat(directory, file);
			FILE *fd = fopen(filepath, "r");
			if (fd != NULL) {
				printf("555555\n");
				char *current_buffer[BUFFER_SIZE] = {0};
				int bytes_read = fread(current_buffer, 1, BUFFER_SIZE, fd);
				if (bytes_read > 0) {
					sprintf(response, 
					"HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %zu\r\n\r\n%s",
					bytes_read, current_buffer);

					send(client_fd, response, strlen(response), 0);
				}
			} else {
					printf("66666\n");
					char *reply = "HTTP/1.1 404 Not Found\r\n\r\n";
					send(client_fd, reply, strlen(reply), 0);
			} 
		} 
	} else {
		printf("5\n");
		char *reply = "HTTP/1.1 404 Not Found\r\n\r\n";
	    send(client_fd, reply, strlen(reply), 0);
	}
	printf("6\n");
	//cloose the client connection
	close(client_fd);
	close(server_fd);
	return 0;
}
