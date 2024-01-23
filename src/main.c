#include <stdio.h>
#include "kmake.h"
#include "args.h"
#include "strings.h"

int main(int argc, char** argv) {
	const char* progname = getarg(argc, argv);
	printf("Running: %s\n", progname);

	if (shiftargs(&argc, &argv)) {
		run_with_args(argc, argv);
	} else{
		make();
	}
}
