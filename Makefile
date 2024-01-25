CC:=clang
srcdir:=./src
builddir:=./.build
objdir:=obj
target:=kmake
ifeq ($(OS), WINDOWS_NT)
	target:=kmake.exe
endif

srcfiles:=$(wildcard $(srcdir)/*.c)
headerfiles:=$(wildcard $(srcdir)/*.h)
objfiles:=$(patsubst $(srcdir)/%.c,$(builddir)/$(objdir)/%.c.o, $(srcfiles))

INC_dirs:=$(addprefix -I,$(srcdir))
CFlags:=-x c -std=c17 -g -O0 -Werror -Wall -pedantic $(INC_dirs)
LDFlags:=

all: $(target)

.PHONY: run cleanW clean dirW dir install

run:
	./$(target)

install: $(target)
	install -v ./$(target) /usr/local/bin

cleanW:
	del $(builddir)\$(objdir)\*.o
	del $(target)
clean:
	rm $(builddir)/$(objdir)/*.o
	rm $(target)

dirW:
	mkdir $(builddir)\$(objdir)
dir:
	mkdir $(builddir)/$(objdir)

$(builddir)/$(objdir)/%.c.o: $(srcdir)/%.c Makefile $(headerfiles)
	$(CC) $(CFlags) -c $< -o $@

$(target): Makefile $(objfiles) $(headerfiles)
	$(CC) $(objfiles) -o $@ $(LDFlags)
