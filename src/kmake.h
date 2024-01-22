#ifndef _H_KMAKE
#define _H_KMAKE

struct Config {
	const char * name;
	const char * d_src;
	const char * d_build;
	const char * cc;
	const char * flags;
	const char * includes;
};

void runarg(int argc, char** argv);
void doMake(void);

void init_dir(void);
void clean_dir(void);
void run_exe(void);
void get_compile_commands(void);

struct Config get_conf(void);


#endif // !_H_KMAKE
