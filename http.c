#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <limits.h>

#include "http.h"

int parse_http_request(const char *buffer, size_t buffer_size, int client_socket){
	const char *start_of_path = strchr(buffer, ' ') + 1;
	const char *end_of_path = strchr(start_of_path, ' ');

	size_t path_len = end_of_path - start_of_path;
	char *path = malloc(sizeof(path_len));
	if(path == NULL){
		return -1;
	}

	strncpy(path, start_of_path, path_len);
	path[strlen(path)] = 0;

	printf("%s\n", path);

	char actual_path[PATH_MAX];
	if(getcwd(actual_path, PATH_MAX) != NULL){
		printf("%s\n", actual_path);
	}

	strncat(actual_path, path, PATH_MAX);
	printf("actual path : %s\n", actual_path);

	free(path);
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