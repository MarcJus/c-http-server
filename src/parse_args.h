#ifndef _PARSE_ARGS_
#define _PARSE_ARGS_

#include <stdint.h>

enum setting_type {
	TYPE_STRING, TYPE_INT
};

struct server_settings {
	const char *name;

	enum setting_type type;

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

int get_int_setting(int setting_index);

char *get_string_setting(int setting_index);

void free_settings();

#endif //_PARSE_ARGS_