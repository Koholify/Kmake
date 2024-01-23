CC:=clang
srcdir:=./src
builddir:=./.build
objdir:=obj
target:=kmake

srcfiles:=$(wildcard $(srcdir)/*.c)
objfiles:=$(patsubst $(srcdir)/%.c,$(builddir)/$(objdir)/%.c.o, $(srcfiles))

INC_dirs:=$(addprefix -I,$(srcdir))
CFlags:=-std=c17 -g -O0 -Werror -Wall -pedantic $(INC_dirs)
LDFlags:=

all: $(target)

.PHONY: run clean dir

run:
	./$(target)

clean:
	del $(builddir)\$(objdir)\*.o
	del $(target)

dir:
	mkdir $(builddir)\$(objdir)

$(builddir)/$(objdir)/%.c.o: $(srcdir)/%.c Makefile
	$(CC) $(CFlags) -c $< -o $@

$(target): Makefile $(objfiles)
	$(CC) $(objfiles) -o $@ $(LDFlags)
