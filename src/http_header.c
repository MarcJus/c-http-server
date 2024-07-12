#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "http_header.h"

#define HTTP_200_RESPONSE_BASE 	"HTTP/1.1 200 OK\r\n"\
								"\r\n"

#define HTTP_404_RESPONSE		"HTTP/1.1 404 Not Found\r\n"\
								"Content-Type: text/plain\r\n"\
								"\r\n"\
								"Not Found\r\n"

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

	return EXIT_SUCCESS;
}