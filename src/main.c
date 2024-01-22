#include <stdio.h>
#include "args.h"
#include "kmake.h"


int main(int argc, char** argv) {
	const char* progname = getarg(argc, argv);
	printf("Running: %s\n", progname);

	if (shiftargs(&argc, &argv)) {
		runarg(argc, argv);
	} else{
		doMake();
	}
}
