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
#include "http_header.h"

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
	size_t response_len = 0;

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

	struct stat file_stat;
	fstat(file_fd, &file_stat);
	off_t file_size = file_stat.st_size;

	memcpy(response, HTTP_200_RESPONSE_BASE, sizeof(HTTP_200_RESPONSE_BASE) - 1);
	response_len += sizeof(HTTP_200_RESPONSE_BASE) - 1;

	ssize_t bytes_read = read(file_fd, response + response_len, HTTP_BUFFER_SIZE - response_len);
	if(bytes_read < 0){
		perror("Impossible de lire le fichier");
		free(response);
		return -1;
	}

	response_len += bytes_read;

	send(client_socket, response, response_len, 0);

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

char *build_response(const char *path, size_t *buf_len){
	char *response = NULL;

	int file_fd = open_file(path);
	if(file_fd < 0){
		perror("Impossible d'ouvrir le fichier");
		return NULL;
	}

	struct stat file_stat;
	if(fstat(file_fd, &file_stat) < 0){
		perror("Impossible de récupérer les statistiques");
		return NULL;
	}

	off_t file_size = file_stat.st_size;
	*buf_len = sizeof(HTTP_200_RESPONSE_BASE) + file_size;

	response = malloc(*buf_len);
	if(response == NULL){
		*buf_len = 0;
		return NULL;
	}
	bzero(response, *buf_len);

	off_t bytes_copied = 0;
	memcpy(response, HTTP_200_RESPONSE_BASE, sizeof(HTTP_200_RESPONSE_BASE) - 1);
	bytes_copied += sizeof(HTTP_200_RESPONSE_BASE) - 1;

	ssize_t bytes_read = read(file_fd, response + bytes_copied, file_size);
	if(bytes_read < 0){
		perror("Impossible de copier les données");
		free(response);
		*buf_len = 0;
		return NULL;
	}

	return response;
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
		char *path = get_request_path(buffer);
		if(path == NULL){
			return -1;
		}
		size_t buf_len;
		char *response = build_response(path, &buf_len);
		if(response == NULL){
			free(path);
			return -1;
		}
		printf("taille : %ld\n", buf_len);
		printf("%s\n", response);

		ssize_t bytes_sent = send(client_socket, response, buf_len, 0);
		if(bytes_sent < 0){
			perror("Erreur lors de l'envoi de la réponse");
			free(path);
			free(response);
			return -1;
		}
		printf("Envoyé : %ld\n", bytes_sent);
		free(path);
		free(response);
	}

	free(buffer);
	return bytes_read;
}