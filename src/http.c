#ifdef __linux__
#define _GNU_SOURCE
#include <linux/limits.h>
#include <sched.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <limits.h>

#include <regex.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "http.h"
#include "file.h"
#include "http_header.h"
#include "parse_args.h"
#include "thread_pool.h"

#define HTTP_BUFFER_SIZE	2048

#define HTTP_200_RESPONSE_BASE 	"HTTP/1.1 200 OK\r\n"\
							   	"Content-Type: text/html\r\n"\
							   	"\r\n"

#define HTTP_404_RESPONSE		"HTTP/1.1 404 Not Found\r\n"\
								"Content-Type: text/plain\r\n"\
								"\r\n"\
								"Not Found\r\n"

char *build_response(const char *path, size_t *buf_len){
	char *response = NULL;

	int file_fd = open_file(path);
	if(file_fd < 0){
		perror("Impossible d'ouvrir le fichier");
		response = malloc(sizeof(HTTP_404_RESPONSE) - 1);
		if(response == NULL)
			return NULL;

		memcpy(response, HTTP_404_RESPONSE, sizeof(HTTP_404_RESPONSE) - 1);
		*buf_len = sizeof(HTTP_404_RESPONSE) - 1; // -1 pour enlever le \0 (il est en trop dans en http)

		return response;
	}

	struct stat file_stat;
	if(fstat(file_fd, &file_stat) < 0){
		perror("Impossible de récupérer les statistiques");
		close(file_fd);
		return NULL;
	}

	off_t file_size = file_stat.st_size;
	*buf_len = sizeof(HTTP_200_RESPONSE_BASE) + file_size;

	response = malloc(*buf_len);
	if(response == NULL){
		*buf_len = 0;
		close(file_fd);
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
		close(file_fd);
		return NULL;
	}

	response[*buf_len - 1] = '\n';
	close(file_fd);
	return response;
}

void *read_http_request(void *arg){
	int retval;
	char *root;

	d("thread créé\n");

	char *buffer = malloc(HTTP_BUFFER_SIZE);
	if(buffer == NULL){
		return NULL;
	}
	bzero(buffer, HTTP_BUFFER_SIZE);
	#ifdef __linux__
	unshare(CLONE_FS);
	#endif

	root = get_string_setting(SETTING_ROOT);
	if(root != NULL && strcmp(root, "")){ // renvoie 0 si égaux : on entre dans la condition s'ils sont différents
		retval = chdir(root);
		if(retval < 0){
			perror("Erreur chdir");
			return NULL;
		}
	}

	int client_socket;
	while(1){
		client_socket = get_client();
		if(client_socket == 0) // pas le bon thread
			continue;

		ssize_t bytes_read = recv(client_socket, buffer, HTTP_BUFFER_SIZE - 1, 0);
		if(bytes_read < 0){
			perror("Impossible de recevoir les données");
		} else if(bytes_read > 0) {
			char *path = get_request_path(buffer);
			if(path == NULL){
				printf("Erreur get_request_path\n");
				close(client_socket);
				continue;
			}
			d("path : /%s\n", path);

			size_t buf_len;
			char *response = build_response(path, &buf_len);
			if(response == NULL){
				free(path);
				close(client_socket);
			}
			d("taille : %ld\n", buf_len);

			ssize_t bytes_sent = send(client_socket, response, buf_len, 0);
			if(bytes_sent < 0){
				perror("Erreur lors de l'envoi de la réponse");
				free(path);
				close(client_socket);
			}
			
			d("Envoyé : %ld\n", bytes_sent);
			free(path);
			free(response);
		}
		close(client_socket);
		bzero(buffer, HTTP_BUFFER_SIZE); // vide le buffer pour le prochain client
	}

	return NULL;
}