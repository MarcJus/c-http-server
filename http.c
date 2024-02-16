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

#include "http.h"

int parse_http_request(char *buffer, size_t buffer_size, int client_socket){

	regex_t regex;
	regcomp(&regex, "^GET /([^ ]*) HTTP/([^ ]*)", REG_EXTENDED);
	regmatch_t matches[2];

	if(regexec(&regex, buffer, 3, matches, 0) == 0){
		buffer[matches[1].rm_eo] = '\0';
		const char *file_name = buffer + matches[1].rm_so;
	}
	
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