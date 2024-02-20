#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

#include "file.h"

#define INDEX_HTML "index.html"

int open_file(const char *file_name){
	int fd;

	if(strcmp(file_name, "")){ // Si la requête est /
		return open("index.html", O_RDONLY);
	}

	fd = open(file_name, O_RDONLY | O_NONBLOCK);
	if(fd < 0){
		return fd;
	}
	struct stat fd_stat;
	if(fstat(fd, &fd_stat) < 0){
		return -1;
	}
	if(S_ISREG(fd_stat.st_mode)){
		return fd;
	} else {
		return openat(fd, INDEX_HTML, O_RDONLY|O_NONBLOCK);
	}

	return fd;
}