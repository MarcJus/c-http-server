#ifndef _PARSE_ARGS_
#define _PARSE_ARGS_

#include <stdint.h>

struct server_settings {
	const char *name;

	void *value;
};

// Options qui requièrent un argument

extern struct server_settings settings[];

#define SETTING_PORT 	0
#define SETTING_ROOT	1

// Options type "flag"

extern uint8_t settings_flags;

#define FLAG_VERBOSE	0X1


int parse_args(int argc, char* const* argv);

void free_settings();

#endif //_PARSE_ARGS_