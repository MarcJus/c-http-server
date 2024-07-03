#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "parse_args.h"

struct server_settings settings[] = {
	{ "port", TYPE_INT },
	{ "root", TYPE_STRING },
	{}
};

uint8_t settings_flags = 0;

void print_usage(){
	printf("usage : [-v] [-r root] [port]");
}

int is_root_valid(char *root){
	int fd = open(root, O_RDONLY);
	if(fd < 0)
		return 0;

	struct stat fd_stat;
	if(fstat(fd, &fd_stat) < 0){
		close(fd);
		return 0;
	}

	if(!S_ISDIR(fd_stat.st_mode)){
		close(fd);
		return 0;
	}

	close(fd);
	return 1;
}

int parse_args(int argc, char* const* argv){
	int optval = 0;
	char *short_options = "r:v";
	const struct option long_options[] = {
		{ "root",		required_argument,	NULL, 'r' },
		{ "verbose",	no_argument, 		NULL, 'v' },
		{ NULL, 		no_argument, 		NULL, '\0'}
	};
	int root_flag = 0;

	while((optval = getopt_long(argc, argv, short_options, long_options, NULL)) != EOF){

		switch(optval){
			case 'r':
				size_t root_len = strlen(optarg);
				if(!is_root_valid(optarg)){
					printf("Invalid root\n");
					return EXIT_FAILURE;
				}
				
				char *root = malloc(root_len) + 1; // +1 pour le \0
				if(root == NULL){
					return EXIT_FAILURE;
				}

				strncpy(root, optarg, root_len);
				root[root_len] = '\0';
				printf("root : %s\n", root);
				root_flag = 1;

				settings[SETTING_ROOT].value = root;
				break;

			case 'v':
				settings_flags |= FLAG_VERBOSE;
				break;

			default:
				print_usage();
				return EXIT_FAILURE;
		}

	}

	if(!root_flag) { // par défaut, la racine des fichiers HTML est dans le même dossier que l'executable
		char *root = malloc(1);
		if(root == NULL)
			return EXIT_FAILURE;

		*root = '\0';
		settings[SETTING_ROOT].value = root;
	}

	int *port = malloc(sizeof(int));
	if(port == NULL){
		return EXIT_FAILURE;
	}

	if((optind + 1) > argc) { // aucun argument : on met le port à 80 par défaut
		*port = 80;
	} else {
		int value = atoi(argv[optind]);
		if(value == 0){
			print_usage();
			free(port);
			return EXIT_FAILURE;
		}

		*port = value;
	}
	settings[SETTING_PORT].value = port;

	return 0;
}

void free_settings(){
	int i = 0;
	while (settings[i].name != NULL){
		free(settings[i].value);
		(&(settings[i]))->value = NULL;
		i++;
	}
}

int get_int_setting(int setting_index){
	struct server_settings setting = settings[setting_index];
	if(setting.type != TYPE_INT)
		return -1;

	return *((int *)setting.value);
}

char *get_string_setting(int setting_index){
	struct server_settings setting = settings[setting_index];
	if(setting.type != TYPE_STRING)
		return NULL;

	return (char *)setting.value;
}
