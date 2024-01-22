#include <stdio.h>
#include "args.h"

int main(int argc, char** argv) {
	const char* progname = getarg(argc, argv);
	printf("Running: %s\n", progname);

	while (shiftargs(&argc, &argv)) {
		const char* arg = getarg(argc, argv);
		printf("%s\n", arg);
	}
}
