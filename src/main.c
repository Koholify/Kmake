#include "kmake.h"
#include "args.h"
#include "strings.h"

int main(int argc, char** argv) {
	if (shiftargs(&argc, &argv)) {
		run_with_args(argc, argv);
	} else{
		make();
	}
}
