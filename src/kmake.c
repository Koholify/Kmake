#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include "kmake.h"
#include "strings.h"
#include "args.h"

#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__NT__)
#define WINDOWS_VER 1

#include <Windows.h>
#define PATH_MAX 4096

#else

#define POSIX_VER 1
#define __USE_MISC 1
#include <stdint.h>
#include <err.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>

#ifdef __linux__
#include <linux/limits.h>
#endif

#endif


struct cmd_ptrs {
	const char* file;
	const char* obj_path;
	// string to be filled.
	char* out;
	// max length of out.
	int n;
};

typedef struct cmd_ptrs cmd_ptrs;
const char* helloworldTemplate = "\
#include <stdio.h>\n\
\n\
int main(int argc, char** argv) {\n\
	printf(\"Hello World!\\n\");\n\
}\n";

const char* makeTemplate = "\
CC=clang\n\
SRC=./src/\n\
BUILD=./.build/\n\
\n\
PROJECTNAME=app\n\
CFLAGS=-std=c17 -Werror -Wall -pedantic\n\
LFLAGS=\n\
INCLUDES=\n\
\n\
INSTALL_LOC=/usr/bin/\n\
COMPILE_COMMANDS=0\n\
";

const char* gitignoreTemplate = "\
.build\n\
.vs\n\
.vscode\n\
";

#if WINDOWS_VER
static str_array get_files_windows(const char* dir_path);
#else 
static str_array get_files_posix(const char* dir_path);
#endif

str_array get_files(const char* dir) {
#if WINDOWS_VER
	return get_files_windows(dir);
#else 
	return get_files_posix(dir);
#endif
}

const char* get_cwd(void)
{
	char* cwd = 0;
	char path[PATH_MAX];
#if WINDOWS_VER
	GetCurrentDirectoryA(PATH_MAX, path);
#else
	getcwd(path, PATH_MAX);
#endif

	if (str_ends_with(path, "/") || str_ends_with(path, "\\")){
		cwd = str_acopy(path);
	} else {
		cwd = str_cat(path, "/");
	}

	return cwd;
}

static const char* get_target(struct Config* config) {
	char target[PATH_MAX];
	char* extension = "";
#if WINDOWS_VER
	extension = ".exe";
#endif
	snprintf(target, PATH_MAX, "%s%s%s", config->d_build, config->name, extension);
	return str_acopy(target);
}

static void print_err_msg(void) {
		printf("No proper agrument.\n"
			"Try: kmake <command:optional>\n"
			"commands:\n"
			"\tinit\n"
			"\tclean\n"
			"\trun\n"
			"\tinstall\n"
			"\tcmd\n");
}

static void create_compile_command(struct Config* config, cmd_ptrs* ptrs) {
	char outfile[PATH_MAX];
	snprintf(outfile, PATH_MAX, "%s%s.o", ptrs->obj_path, ptrs->file);
	snprintf(ptrs->out, ptrs->n, "%s %s %s -I%s -c %s%s -o %s", config->cc, config->cflags, config->includes, config->d_src, config->d_src, ptrs->file, outfile);
}

static int create_directory(const char* path) {
	int result = 1;
#if WINDOWS_VER
	result = CreateDirectoryA(path, 0) ? 0 : 1;
#else
	result = mkdir(path, 0777);
#endif
	return result;
}

void run_with_args(int argc, char** argv) {
	const char* arg = getarg(argc, argv);
	if (!arg) {
		return;
	}

	struct Config config = get_config();
	if (!str_eql(arg, "init") && (!config.cc || !config.name)) {
		printf("KMakeFile.txt not found or unable to open\n"); 
		abort();
	}

	if (str_eql(arg, "init")) {
		init_dir();
	} else if (str_eql(arg, "clean")) {
		clean_dir(&config);
	} else if (str_eql(arg, "run")) {
		run_exe(&config);
	} else if (str_eql(arg, "cmd")) {
		get_compile_commands(&config);
	} else if (str_eql(arg, "install")) {
		install(&config);
	} else {
		print_err_msg();
	}

	free_config(config);
}

void make(void) {
	struct Config config = get_config();
	if (!config.cc || !config.name) {
		printf("KMakeFile.txt not found or unable to open\n"); 
		abort();
	}

	str_array source_files = get_source_files(config.d_src);

	if (source_files.length < 1) {
		puts("No Source Files Found");
		abort();
	}

	char obj_path[PATH_MAX];
	snprintf(obj_path, PATH_MAX, "%sobj/", config.d_build);

	char outfiles[PATH_MAX];
	char cmd[PATH_MAX] = {0};
	cmd_ptrs ptrs = {.n = PATH_MAX, .file = 0, .obj_path = obj_path, .out = cmd};
	for(int i = 0; i < source_files.length; i++) {
		ptrs.file = source_files.array[i];

		snprintf(outfiles, PATH_MAX, "%s%s", config.d_src, ptrs.file);
		unsigned long long msource = get_file_mtime(outfiles);
		snprintf(outfiles, PATH_MAX, "%s%s.o", obj_path, ptrs.file);
		unsigned long long mobj = get_file_mtime(outfiles);

		if (msource > mobj) {
			create_compile_command(&config, &ptrs);
			printf("%s\n", cmd);
			int r = system(cmd);
			if (r) {
				printf("File %s failed to compile!!\n", source_files.array[i]);
				return;
			}
		}
	}

	str_stream* ss = str_stream_init();
	str_array object_files = get_object_files(obj_path);
	for(int i = 0; i < object_files.length; i++) {
		snprintf(outfiles, PATH_MAX, "%s%s", obj_path, object_files.array[i]);
		str_stream_add(ss, outfiles);
		str_stream_add(ss, " ");
	}

	const char* obj_files = str_stream_merge(ss);
	str_stream_free(ss);

	const char* target = get_target(&config);
	snprintf(cmd, PATH_MAX, "%s %s-o %s %s", config.cc, obj_files, target, config.lflags);
	printf("%s\n", cmd);
	int r = system(cmd);
	if (r) {
		printf("Executable failed to link!!\n");
		return;
	}

	free((void*)target);
	free((void*)obj_files);
	str_array_free(&source_files);
	str_array_free(&object_files);

	if (config.compile_command) {
		get_compile_commands(&config);
	}

	free_config(config);
}

void init_dir(void) {
	printf("Adding source and build directories...\n");
	int create_source = 0;
	if (create_directory("./src"))
		printf("Unable to make \"%s\" directory.\n", "./src");
	if (create_directory("./.build")) 
		printf("Unable to make \"%s\" directory.\n", "./.build");
	else if (create_directory("./.build/obj"))
		printf("Unable to make \"%s\" directory.\n", "./.build/obj");

	const char* cwd = get_cwd();
	printf("Creating config file:\nDIR=%s\n%s\n", cwd, makeTemplate);
	FILE* conf = fopen("KMakeFile.txt", "w");
	fputs("DIR=", conf);
	fputs(cwd, conf);
	fputc('\n', conf);
	fputs(makeTemplate, conf);
	fclose(conf);
	free((void*)cwd);

	printf("Creating .gitignore file:\n%s\n", gitignoreTemplate);
	FILE* ignore = fopen(".gitignore", "w");
	fputs(gitignoreTemplate, ignore);
	fclose(ignore);

	if (!create_source) {
		FILE* example = fopen("./src/main.c", "w");
		fputs(helloworldTemplate, example);
		fclose(example);
	}
}

void clean_dir(struct Config* config) {

	printf("removing files...\n");
	const char* target = get_target(config);

	if (remove(target)) {
		printf("Failed to remove %s\n", target);
	}
	free((void*)target);
	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%sobj/", config->d_build);

	str_array objs = get_object_files(path);
	char file[PATH_MAX];
	for (int i = 0; i < objs.length; i++) {
		snprintf(file, PATH_MAX, "%s%s", path, objs.array[i]);
		if (remove(file)) {
			printf("Failed to remove %s\n", file);
		}
	}

	str_array_free(&objs);
}

void run_exe(struct Config* config) {
	const char* target = get_target(config);

#if WINDOWS_VER
	char* winTarget = (char*)malloc(sizeof(char) * (str_len(target) + 1));
	for(int i = 0; i < str_len(target) + 1; i++) {
		winTarget[i] = target[i];
		if (target[i] == '/') winTarget[i] = '\\';
	}
	free((void*)target);
	target = winTarget;
#endif
	printf("%s\n", target);
	system(target);
	free((void*)target);
}

void get_compile_commands(struct Config* config) {
	printf("Creating compile_commands.json\n");
	FILE* file = fopen("compile_commands.json", "w");
	if (!file) {
		printf("Unable to create compile_commands.json\n");
		return;
	}

	const char* start = "[\n";
	const char* end = "]\n";
	const char* commandTemplate = "\
{\n\
  \"directory\": \"%s\",\n\
  \"command\": \"%s\",\n\
  \"file\": \"%s\",\n\
  \"output\": \"%s%s.o\"\n\
}";

	str_array source_files = get_files(config->d_src);
	char cmd[1024];
	char obj_path[PATH_MAX]; 
	snprintf(obj_path, PATH_MAX, "%sobj/", config->d_build);

	fputs(start, file);

	for (int i = 0; i < source_files.length; i++) {
		cmd_ptrs ptrs = {.out = cmd, .obj_path = obj_path, .file = source_files.array[i], .n = 1024};
		create_compile_command(config, &ptrs);
	
		char filledTemplate[4096];
		snprintf(filledTemplate, 4096, commandTemplate, config->d_parent, cmd, source_files.array[i], obj_path, source_files.array[i]);
		fputs(filledTemplate, file);

		if (i == source_files.length  - 1) {
			fputs("\n", file);
		} else {
			fputs(",\n", file);
		}
	}
	fputs(end, file);

	fclose(file);

	str_array_free(&source_files);
}

void install(struct Config* config) {
	char cmd[PATH_MAX];
	const char* target = get_target(config);
#if WINDOWS_VER
	snprintf(cmd, PATH_MAX, "copy %s %s", target, config->d_install);
	for(int i = 0; i < str_len(cmd); i++)
		if (cmd[i] == '/') cmd[i] = '\\';
#else
	snprintf(cmd, PATH_MAX, "install -v %s %s", target, config->d_install);
#endif

	printf("%s\n", cmd);
	system(cmd);
	free((void*)target);
}

static void FillConfig(struct Config* config, struct str_array* lines);
struct Config get_config(void) {
	FILE* file = fopen("KMakeFile.txt", "r");
	if (!file) {
		return (struct Config){0};
	}

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
	free((void*)config.d_install);
	free((void*)config.d_parent);
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
			if (str_eql(split.array[0], "INSTALL_LOC"))
				config->d_install = str_acopy(split.array[1]);
			if (str_eql(split.array[0], "DIR"))
				config->d_parent = str_acopy(split.array[1]);
			if (str_eql(split.array[0], "COMPILE_COMMANDS"))
				config->compile_command = str_eql(split.array[1], "1");
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

		if (!(str_ends_with(config->d_parent, "/") || str_ends_with(config->d_parent, "\\"))) {
			const char* old = config->d_parent;
			config->d_parent = str_cat(old, "/");
			free((void*)old);
		}

		if (!(str_ends_with(config->d_install, "/") || str_ends_with(config->d_install, "\\"))) {
			const char* old = config->d_install;
			config->d_install = str_cat(old, "/");
			free((void*)old);
		}

		str_array_free(&split);
	}
}

#if WINDOWS_VER
static str_array get_files_windows(const char* dir_path) {
	WIN32_FIND_DATAA data = {0};
	char* dir = str_cat(dir_path, "*");

	int filecount = 0;
	HANDLE h = FindFirstFileA(dir, &data);

	if (h != INVALID_HANDLE_VALUE) {
		do {
			filecount++;
		} while( FindNextFileA(h, &data));
	}

	FindClose(h);


	str_array array = {0};
	array.length = filecount;
	array.array = (char**)malloc(sizeof(void*) * array.length);

	h = FindFirstFileA(dir, &data);
	int i = 0;
	if (h != INVALID_HANDLE_VALUE) {
		do {
			array.array[i++] = str_acopy(data.cFileName);
		} while (FindNextFileA(h, &data));
	}

	FindClose(h);
	free(dir);

	return array;
}
#endif

#if POSIX_VER
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

unsigned long long get_file_mtime(const char* filename) {
#if WINDOWS_VER
	BY_HANDLE_FILE_INFORMATION info = {0};
	HANDLE hf = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	GetFileInformationByHandle(hf, &info);
	CloseHandle(hf);

	ULARGE_INTEGER ul;
	ul.LowPart = info.ftLastWriteTime.dwLowDateTime;
	ul.HighPart = info.ftLastWriteTime.dwHighDateTime;

	return ul.QuadPart;
#else
	struct stat st = {0};
	if (stat(filename, &st)) {
		return 0;
	}
	return (unsigned long long)st.st_mtime;
#endif
	return 0;
}
