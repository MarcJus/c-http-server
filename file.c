#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "file.h"

#define INDEX_HTML "index.html"

int open_file(const char *file_name){
	int file_fd;

	if(strcmp(file_name, "")){ // Si la requête est /
		return open("index.html", O_RDONLY);
	}

	file_fd = open(file_name, O_RDONLY);
	if(file_fd < 0){
		if(errno == ENOENT){
			size_t new_path_len = sizeof(INDEX_HTML) + strlen(file_name);
			char *new_path = malloc(sizeof(INDEX_HTML) + strlen(file_name));
			if(new_path == NULL){
				return -1;
			} else {
				bzero(new_path, new_path_len);
				strncpy(new_path, file_name, strlen(file_name));
				strncat(new_path, INDEX_HTML, new_path_len);

				file_fd = open(new_path, O_RDONLY);
			}
		}
	}

	return file_fd;
}