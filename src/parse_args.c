#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "parse_args.h"

struct server_settings settings[] = {
	{ "port" },
	{ "root" },
	{}
};

int parse_args(int argc, char const* argv[]){
	int optval = 0;
	char *short_options = "r:v";
	const struct option long_options[] = {
		{ "root",		required_argument,	NULL, 'r' },
		{ "verbose",	no_argument, 		NULL, 'v' },
		{ NULL, 		no_argument, 		NULL, '\0'}
	};

	while((optval = getopt_long(argc, argv, short_options, long_options, NULL)) != EOF){

	}

	return 0;
}
