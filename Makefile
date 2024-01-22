all: run

.PHONY: run clean

run:
	@echo Hello

clean:
	del .\.build\*.txt
