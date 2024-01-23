#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include "kmake.h"
#include "strings.h"
#include "args.h"

#ifdef __WIN32__
#include <windows.h>
#else
#include <dirent.h>
#endif

static void print_err_msg(void) {
		printf("No proper agrument.\n"
			"Try: kmake <command:optional>\n"
			"commands:\n"
			"\tinit\n"
			"\tclean\n"
			"\trun\n"
			"\tcmd\n");
}

void runarg(int argc, char** argv) {
	const char* arg = getarg(argc, argv);
	if (!arg) {
		return;
	}

	if (str_eql(arg, "init")) {
		init_dir();
	} else if (str_eql(arg, "clean")) {
		clean_dir();
	} else if (str_eql(arg, "run")) {
		run_exe();
	} else if (str_eql(arg, "cmd")) {
		get_compile_commands();
	} else {
		print_err_msg();
	}
}

void doMake(void) {
	struct Config config = get_config();
	printf("Config:\n\tNAME: %s\n\tCC: %s\n\tSRC: %s\n\tBUILD: %s\n\tCFLAGS: %s\n\tLFLAGS: %s\n\tINCLUDES: %s\n", 
			config.name, config.cc, config.d_src, config.d_build, config.cflags, config.lflags, config.includes);

	str_array source_files = get_source_files(config.d_src);
	str_array object_files = get_object_files(config.d_build);

	free_config(config);
}

void init_dir(void) {
}

void clean_dir(void) {
}

void run_exe(void) {
}

void get_compile_commands(void) {
}

static void FillConfig(struct Config* config, struct str_array* lines);
struct Config get_config(void) {
	FILE* file = fopen("KMakeFile.txt", "r");
	str_stream* ss = str_stream_init();

	char buffer[256] = {0};
	while(fgets(buffer, 256, file))
	{
		str_stream_add(ss, buffer);
	}
	fclose(file);

	char* contents = str_stream_merge(ss);
	str_stream_free(ss);

	struct str_array lines = str_split(contents, "\n");
	free(contents);

	struct Config config = {0};
	FillConfig(&config, &lines);
	
	free(lines.array);
	return config;
}

void free_config(struct Config config) {
	free((void*)config.name);
	free((void*)config.d_src);
	free((void*)config.d_build);
	free((void*)config.cc);
	free((void*)config.cflags);
	free((void*)config.lflags);
	free((void*)config.includes);
}

static void FillConfig(struct Config* config, struct str_array* lines) {
	char** array = lines->array;
	int length = lines->length;
	for (int i = 0; i < length; i++) {
		str_array split = str_splitN(array[i], "=", 1);
		if (split.length == 2)
		{
			if (str_eql(split.array[0], "PROJECTNAME"))
				config->name = str_acopy(split.array[1]);
			if (str_eql(split.array[0], "CFLAGS"))
				config->cflags = str_acopy(split.array[1]);
			if (str_eql(split.array[0], "LFLAGS"))
				config->lflags = str_acopy(split.array[1]);
			if (str_eql(split.array[0], "INCLUDES"))
				config->includes = str_acopy(split.array[1]);
			if (str_eql(split.array[0], "SRC"))
				config->d_src = str_acopy(split.array[1]);
			if (str_eql(split.array[0], "CC"))
				config->cc = str_acopy(split.array[1]);
			if (str_eql(split.array[0], "BUILD"))
				config->d_build = str_acopy(split.array[1]);
		}
	}
}
