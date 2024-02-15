#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "http.h"

int parse_http_request(char *buffer, size_t size, int client_socket){
	char *request_line = strtok(buffer, "\n");
	if(request_line == NULL){
		return -1;
	} else {
		printf("%s\n", request_line);
	}

	char *path = strtok(request_line, " ");
	while(path != NULL && !strstr(path, "/")){
		path = strtok(NULL, " ");
	}

	if(path == NULL)
		return -1;

	printf("%s\n", path);

	return 0;
}

int read_http_request(int client_socket){
	while(1){
		char *buffer = malloc(1024);
		if(buffer == NULL){
			return -1;
		}
		bzero(buffer, 1024);
		ssize_t bytes_read = recv(client_socket, buffer, 1023, 0);
		if(bytes_read < 0){
			return -1;
		} else if (bytes_read == 0){
			break;
		} else {
			printf("%s", buffer);
		}

		parse_http_request(buffer, strlen(buffer), client_socket);

	}

	return 0;
}