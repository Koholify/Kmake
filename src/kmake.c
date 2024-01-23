#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include "kmake.h"
#include "strings.h"
#include "args.h"

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
	get_conf();
}

void init_dir(void) {
}

void clean_dir(void) {
}

void run_exe(void) {
}

void get_compile_commands(void) {
}

struct Config get_conf(void) {
	FILE* file = fopen("KMakeFile.txt", "r");
	char * contents = 0;
	printf("Got file\n");

	char buffer[256] = {0};
	while(fgets(buffer, 256, file))
	{
		printf("reading: %s", buffer);
		char* tmp = str_cat(contents, buffer);
		free(contents);
		contents = tmp;
	}

	printf("Close file\n");
	fclose(file);

	printf("Result:\n%s...\n", contents);

	struct str_array lines = str_split(contents, "\n");

	
	free(lines.array);
	return (struct Config){0};
}
