#ifndef _PARSE_ARGS_
#define _PARSE_ARGS_

struct server_settings {
	const char *name;

	const void *value;
};

// Options qui requièrent un argument

extern struct server_settings settings[];

#define SETTING_PORT 	0
#define SETTING_ROOT	1

// Options type "flag"

#define FLAG_VERBOSE	0X1


int parse_args(int argc, char const* argv[]);

#endif //_PARSE_ARGS_