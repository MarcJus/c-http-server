#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "http.h"

int parse_http_request(const char *buffer, size_t size, int client_socket){
	
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

		parse_http_socket(buffer, strlen(buffer), client_socket);

	}

	return 0;
}