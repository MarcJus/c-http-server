#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "http_header.h"

#define STATUS_LINE_200_OK				"HTTP/1.1 200 OK\r\n"

#define STATUS_LINE_404_NOT_FOUND		"HTTP/1.1 404 Not Found\r\n"

#define STATUS_LINE_INTERNAL_ERROR		"HTTP/1.1 503 Service Unavailable\r\n"


#define HEADER_CONTENT_TYPE				"Content-Type"

#define COONTENT_TYPE_HTML				"text/html"
#define CONTENT_TYPE_PLAIN				"text/plain"
#define CONTENT_TYPE_CSS				"text/css"
#define CONTENT_TYPE_JS					"text/javascript"
#define CONTENT_TYPE_JSON				"application/json"

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

int build_http_header(struct http_response *response){
	switch(response->status_code){
		case OK:
		{
			if(response->buffer != NULL) // le buffer ne doit pas déjà être initialisé
				return EXIT_FAILURE;

			size_t header_length = strlen(STATUS_LINE_200_OK);
			response->buffer = malloc(header_length);
			if(!response->buffer)
				return EXIT_FAILURE;

			memcpy(response->buffer, STATUS_LINE_200_OK, header_length);

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