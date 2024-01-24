#include <assert.h>
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include "kmake.h"
#include "strings.h"
#include "args.h"

#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__NT__)
#define WINDOWS_VER 1
#include <windows.h>
#else
#define __USE_MISC 1
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

const char* template = "\
CC=clang\n\
SRC=./src/\n\
BUILD=./.build/\n\
\n\
PROJECTNAME=app\n\
CFLAGS=-std=c17 -Werror -Wall -pedantic\n\
LFLAGS=\n\
INCLUDES=\n\
";

void run_with_args(int argc, char** argv) {
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

void make(void) {
	struct Config config = get_config();
	printf("Config:\n\tNAME: %s\n\tCC: %s\n\tSRC: %s\n\tBUILD: %s\n\tCFLAGS: %s\n\tLFLAGS: %s\n\tINCLUDES: %s\n", 
			config.name, config.cc, config.d_src, config.d_build, config.cflags, config.lflags, config.includes);

	str_array source_files = get_source_files(config.d_src);
	for(int i = 0; i < source_files.length; i++) {
		printf("%s\n", source_files.array[i]);
	}

	char obj_path[256];
	snprintf(obj_path, 256, "%sobj/", config.d_build);

	char cmd[1028] = {0};
	char outfile[512] = {0};
	for(int i = 0; i < source_files.length; i++) {
		snprintf(outfile, 512, "%s%s.o", obj_path, source_files.array[i]);
		snprintf(cmd, 1028, "%s %s -c %s%s -o %s", config.cc, config.cflags, config.d_src, source_files.array[i], outfile);
		printf("%s\n", cmd);
		int r = system(cmd);
		if (r) assert(0 && "File failed to compile!!");
	}

	str_stream* ss = str_stream_init();
	str_array object_files = get_object_files(obj_path);
	for(int i = 0; i < object_files.length; i++) {
		snprintf(outfile, 512, "%s%s", obj_path, object_files.array[i]);
		str_stream_add(ss, outfile);
		str_stream_add(ss, " ");
	}

	const char* obj_files = str_stream_merge(ss);
	str_stream_free(ss);

	char* target = str_acopy(config.name);
#if WINDOWS_VER
	char* ntarget = str_cat(target, ".exe");
	free(target);
	target = ntarget;
#endif
	
	snprintf(cmd, 1028, "%s %s-o %s %s", config.cc, obj_files, config.name, config.lflags);
	printf("%s\n", cmd);
	int r = system(cmd);
	if (r) assert(0 && "Executable failed to link!!");

	free(target);
	free((void*)obj_files);
	free_config(config);
	str_array_free(&source_files);
	str_array_free(&object_files);
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
	
	str_array_free(&lines);
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

		if (!(str_ends_with(config->d_build, "/") || str_ends_with(config->d_build, "\\"))) {
			const char* old = config->d_build;
			config->d_build = str_cat(old, "/");
			free((void*)old);
		}

		if (!(str_ends_with(config->d_src, "/") || str_ends_with(config->d_src, "\\"))) {
			const char* old = config->d_src;
			config->d_src = str_cat(old, "/");
			free((void*)old);
		}

		str_array_free(&split);
	}
}

#if WINDOWS_VER
static str_array get_files_windows(const char* dir_path) {
}
#endif

#if defined(__APPLE__) || defined(__linux__)
static str_array get_files_posix(const char* dir_path) {
	DIR* dir = opendir(dir_path);
	struct dirent* dir_info;
	dir_info = readdir(dir);
	int filecount = 0;
	for (; dir_info; dir_info = readdir(dir)){
		if (dir_info->d_type == DT_REG)
			filecount++;
	}
	closedir(dir);

	str_array array = {0};
	array.length = filecount;
	array.array = (char**)malloc(sizeof(void*) * array.length);

	dir = opendir(dir_path);
	dir_info = readdir(dir);
	for (int i = 0; dir_info; dir_info = readdir(dir)){
		if (dir_info->d_type == DT_REG)
			array.array[i++] = str_acopy(dir_info->d_name);
	}

	closedir(dir);
	return array;
}
#endif

static str_array get_files(const char* dir) {
#if WINDOWS_VER
	return get_files_windows(dir);
#else 
	return get_files_posix(dir);
#endif
}

str_array get_source_files(const char* dir) {
	str_array files = get_files(dir);

	int count = 0;
	for (int i = 0; i < files.length; i++) {
		if (str_ends_with(files.array[i], ".c"))
			count++;
	}

	str_array array = {0};
	array.length = count;
	array.array = (char**)malloc(sizeof(void*) * array.length);
	for (int i = 0, j = 0; i < files.length; i++) {
		if (str_ends_with(files.array[i], ".c"))
			array.array[j++] = str_acopy(files.array[i]);
	}

	str_array_free(&files);
	return array;
}

str_array get_object_files(const char* dir) {
	str_array files = get_files(dir);

	int count = 0;
	for (int i = 0; i < files.length; i++) {
		if (str_ends_with(files.array[i], ".o"))
			count++;
	}

	str_array array = {0};
	array.length = count;
	array.array = (char**)malloc(sizeof(void*) * array.length);
	for (int i = 0, j = 0; i < files.length; i++) {
		if (str_ends_with(files.array[i], ".o"))
			array.array[j++] = str_acopy(files.array[i]);
	}

	str_array_free(&files);
	return array;
}

