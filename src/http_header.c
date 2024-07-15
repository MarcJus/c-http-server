#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "http_header.h"

#define STATUS_LINE_200_OK				"HTTP/1.1 200 OK\r\n"

#define STATUS_LINE_404_NOT_FOUND		"HTTP/1.1 404 Not Found\r\n"

#define STATUS_LINE_INTERNAL_ERROR		"HTTP/1.1 503 Service Unavailable\r\n"


#define HEADER_CONTENT_TYPE				"Content-Type"

#define CONTENT_TYPE_HTML				"text/html"
#define CONTENT_TYPE_PLAIN				"text/plain"
#define CONTENT_TYPE_CSS				"text/css"
#define CONTENT_TYPE_JS					"text/javascript"
#define CONTENT_TYPE_JSON				"application/json"

#define HEADER_FIELD_SEPARATOR			" : "

enum header_field {
	HEADER_FIELD_CONTENT_TYPE
};

char *get_request_path(const char *request){
	char *path = NULL;
	regex_t regex;
	regmatch_t matches[3];

	regcomp(&regex, "^(GET|POST) /([^ ]*) HTTP/([^ ]*)", REG_EXTENDED);
	if(regexec(&regex, request, 3, matches, 0) == 0){
		size_t path_len = matches[2].rm_eo - matches[2].rm_so;
		path = malloc(path_len + 1); // +1 pour le \0
		if(path == NULL){
			return NULL;
		}
		memcpy(path, request + matches[2].rm_so, path_len);
		path[path_len] = 0;
	}
	regfree(&regex);

	return path;
}

char *find_file_extension(char *file_name){
	char *delimitor = ".";
	char *token, *last_token = NULL;
	char *saveptr, *str = file_name;

	for(;; str = NULL){
		if((token = strtok_r(str, delimitor, &saveptr)) == NULL)
			break;

		last_token = token;
	}

	return last_token;
}

/**
 * @brief Ajoute un header field au header
 * 
 * @param response 
 * @param field 
 * @return int 
 */
int add_header_field(struct http_response *response, enum header_field field){
	switch(field){
		case HEADER_FIELD_CONTENT_TYPE: // détermine le content type grâce à l'extension du fichier
		{
			char *new_buffer, *content_type, *field;
			size_t to_copy = sizeof(HEADER_CONTENT_TYPE) + sizeof(HEADER_FIELD_SEPARATOR); // ! Surement pas la bonne taille ! sizeof() semble compter le \0

			char *extension = find_file_extension(response->file_name);
			if(!extension)
				return EXIT_FAILURE;

			if(strcmp(extension, "html")){
				content_type = CONTENT_TYPE_HTML;
			} else if (strcmp(extension, "css")){
				content_type = CONTENT_TYPE_CSS;
			} else if (strcmp(extension, "js")){
				content_type = CONTENT_TYPE_JS;
			} else if (strcmp(extension, "json")){
				content_type = CONTENT_TYPE_JSON;
			} else {
				content_type = CONTENT_TYPE_PLAIN;
			}

			to_copy += strlen(content_type) + 2;
			field = malloc(to_copy);
			if(field == NULL)
				return EXIT_FAILURE;

			memcpy(field, HEADER_CONTENT_TYPE, sizeof(HEADER_CONTENT_TYPE)); // ! sizeof() compte le \0

			new_buffer = realloc(response->buffer, response->length + to_copy);
			if(new_buffer = NULL){
				response->length -= to_copy;
				return EXIT_FAILURE;
			}
			break;
		}
	}
	return EXIT_SUCCESS;
}

int build_http_header(struct http_response *response){
	switch(response->status_code){
		case OK:
		{
			if(response->buffer != NULL) // le buffer ne doit pas déjà être initialisé
				return EXIT_FAILURE;

			size_t *header_length = sizeof(STATUS_LINE_200_OK);
			response->buffer = malloc(*header_length);
			if(!response->buffer){
				*header_length = 0;
				return EXIT_FAILURE;
			}

			memcpy(response->buffer, STATUS_LINE_200_OK, *header_length); // status line

			if(add_header_field(response, HEADER_FIELD_CONTENT_TYPE) == EXIT_FAILURE)
				return EXIT_FAILURE;

			break;
		}
		case NOT_FOUND:
			break;

		case INTERNAL_ERROR:
			break;

		default:
			break;
	}

	return EXIT_SUCCESS;
}