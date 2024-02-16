#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <limits.h>
#ifdef __linux__
#include <linux/limits.h>
#endif
#include <regex.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "http.h"
#include "file.h"

#define HTTP_BUFFER_SIZE	2048

#define HTTP_200_RESPONSE_BASE 	"HTTP/1.1 200 OK\r\n"\
							   	"Content-Type: text/html\r\n"\
							   	"\r\n"

#define HTTP_404_RESPONSE		"HTTP/1.1 404 Not Found\r\n"\
								"Content-Type: text/plain\r\n"\
								"\r\n"\
								"Not Found\r\n"

int send_file(int client_socket, const char *file_name){
	int ret;

	char *response = malloc(HTTP_BUFFER_SIZE);
	if(response == NULL){
		return -1;
	}
	size_t *reponse_len = 0;

	bzero(response, HTTP_BUFFER_SIZE);

	int file_fd = open_file(file_name);
	if(file_fd < 0){
		perror("Impossible d'ouvrir le fichier");
		if(errno == ENOENT){
			snprintf(response, HTTP_BUFFER_SIZE, HTTP_404_RESPONSE);
			send(client_socket, response, strlen(response), 0);
		}
		return -1;
	}

	free(response);
	return ret;
}

int parse_http_request(char *buffer, size_t buffer_size, int client_socket){

	regex_t regex;
	regcomp(&regex, "^GET /([^ ]*) HTTP/([^ ]*)", REG_EXTENDED);
	regmatch_t matches[2];

	if(regexec(&regex, buffer, 2, matches, 0) == 0){
		buffer[matches[1].rm_eo] = '\0';
		const char *file_name = buffer + matches[1].rm_so;

		send_file(client_socket, file_name);
	}

	regfree(&regex);
	return 0;
}

int read_http_request(int client_socket){
	char *buffer = malloc(HTTP_BUFFER_SIZE);
	if(buffer == NULL){
		return -1;
	}
	bzero(buffer, HTTP_BUFFER_SIZE);
	ssize_t bytes_read = recv(client_socket, buffer, HTTP_BUFFER_SIZE - 1, 0);
	if(bytes_read < 0){
		perror("Impossible de recevoir les données");
	} else if(bytes_read > 0) {
		printf("%s", buffer);
		parse_http_request(buffer, strlen(buffer), client_socket);
	}

	free(buffer);
	return bytes_read;
}