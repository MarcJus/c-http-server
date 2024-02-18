#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "http_header.h"

char *get_request_path(const char *request){
	char *path;
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

	return path;
}