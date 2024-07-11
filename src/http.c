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
#include "parse_args.h"

#define HTTP_BUFFER_SIZE	2048

#define HTTP_200_RESPONSE_BASE 	"HTTP/1.1 200 OK\r\n"\
							   	"Content-Type: text/html\r\n"\
							   	"\r\n"

#define HTTP_404_RESPONSE		"HTTP/1.1 404 Not Found\r\n"\
								"Content-Type: text/plain\r\n"\
								"\r\n"\
								"Not Found\r\n"

int build_response(struct http_response *http_response){
	char *path = http_response->file_name;
	size_t *buf_len = &(http_response->length);
	char *response = NULL;

	int file_fd = open_file(path);
	http_response->fd = file_fd;
	if(file_fd < 0){
		perror("Impossible d'ouvrir le fichier");
		response = malloc(sizeof(HTTP_404_RESPONSE) - 1);
		if(response == NULL)
			return EXIT_FAILURE;

		memcpy(response, HTTP_404_RESPONSE, sizeof(HTTP_404_RESPONSE) - 1);
		*buf_len = sizeof(HTTP_404_RESPONSE) - 1; // -1 pour enlever le \0 (il est en trop dans en http)

		http_response->status_code = NOT_FOUND;
	} else {
		struct stat file_stat;
		if(fstat(file_fd, &file_stat) < 0){
			perror("Impossible de récupérer les statistiques");
			close(file_fd);
			http_response->status_code = INTERNAL_ERROR;
		}

		off_t file_size = file_stat.st_size;
		*buf_len = sizeof(HTTP_200_RESPONSE_BASE) + file_size;

		response = malloc(*buf_len);
		if(response == NULL){
			*buf_len = 0;
			close(file_fd);
			http_response->status_code = INTERNAL_ERROR;
			return EXIT_FAILURE; // TODO : Gérer les erreurs 503
		}
		bzero(response, *buf_len);

		off_t bytes_copied = 0;
		memcpy(response, HTTP_200_RESPONSE_BASE, sizeof(HTTP_200_RESPONSE_BASE) - 1);
		bytes_copied += sizeof(HTTP_200_RESPONSE_BASE) - 1;

		ssize_t bytes_read = read(file_fd, response + bytes_copied, file_size);
		if(bytes_read < 0){
			perror("Impossible de copier les données");
			*buf_len = 0;
			free(response);
			response = NULL;
			close(file_fd);
			http_response->status_code = INTERNAL_ERROR;
		}

		http_response->status_code = OK;
		response[*buf_len - 1] = '\n';
		close(file_fd);
	}

	http_response->buffer = response;
	return EXIT_SUCCESS;
}

void *read_http_request(void *arg){
	int retval;
	char *root;

	d("thread créé\n");
	int client_socket;
	if(arg == NULL){
		return NULL;
	}
	client_socket = *(int*)arg;
	free(arg);

	char *buffer = malloc(HTTP_BUFFER_SIZE);
	if(buffer == NULL){
		close(client_socket);
		return NULL;
	}
	bzero(buffer, HTTP_BUFFER_SIZE);

	root = get_string_setting(SETTING_ROOT);
	if(root != NULL && strcmp(root, "")){ // renvoie 0 si égaux : on entre dans la condition s'ils sont différents
		retval = chdir(root);
		if(retval < 0)
			goto ret;
	}

	ssize_t bytes_read = recv(client_socket, buffer, HTTP_BUFFER_SIZE - 1, 0);
	if(bytes_read < 0){
		perror("Impossible de recevoir les données");
	} else if(bytes_read > 0) {
		char *path = get_request_path(buffer);
		if(path == NULL){
			goto ret;
		}
		d("path : /%s\n", path);

		struct http_response http_response = {
			.file_name = path
		};
		if(build_response(&http_response) == EXIT_FAILURE){
			free_http_response(&http_response);
			goto ret;
		}
		d("taille : %ld\n", http_response.length);

		ssize_t bytes_sent = send(client_socket, http_response.buffer, http_response.length, 0);
		if(bytes_sent < 0){
			perror("Erreur lors de l'envoi de la réponse");
			free(path);
			goto ret;
		}
		
		d("Envoyé : %ld\n", bytes_sent);
		free_http_response(&http_response);
	}

ret:
	close(client_socket);
	free(buffer);
	return NULL;
}

void free_http_response(struct http_response *response){
	if(response->buffer){
		free(response->buffer);
		response->buffer = NULL;
	}
	if(response->file_name){
		free(response->file_name);
		response->file_name = NULL;
	}
}