#ifndef _H_KMAKE
#define _H_KMAKE

#include "strings.h"

struct Config {
	const char * name;
	const char * d_src;
	const char * d_build;
	const char * cc;
	const char * cflags;
	const char * lflags;
	const char * includes;
	const char * d_install;
	const char * d_parent;
	char compile_command;
};

void run_with_args(int argc, char** argv);
void make(void);

void init_dir(void);
void clean_dir(struct Config* config);
void run_exe(struct Config* config);
void get_compile_commands(struct Config* config);
void install(struct Config* config);

struct Config get_config(void);
void free_config(struct Config config);

str_array get_source_files(const char* dir);
str_array get_object_files(const char* dir);

#endif // !_H_KMAKE
