#include "args.h"

int shiftargs(int* argc, char*** argv) {
	*argc = *argc - 1;
	(*argv)++;
	return *argc > 0;
}

const char* getarg(int argc, char* const* argv) {
	if (argc < 1) {
		return 0;
	}

	return *argv;
}
