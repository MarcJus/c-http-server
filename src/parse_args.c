#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "parse_args.h"

struct server_settings settings[] = {
	{ "port" },
	{ "root" },
	{}
};

uint8_t settings_flags = 0;

void print_usage(){
	printf("usage : [-v] [-r root] [port]");
}

int parse_args(int argc, char const* argv[]){
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
				return EXIT_FAILURE; // TODO : créer une fonction qui free la variable settings
		}

	}

	return 0;
}
