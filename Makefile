CC:=clang
srcdir:=./src
builddir:=.build
objdir:=obj

srcfiles:=$(wildcard $(srcdir)/*.c)
objfiles:=$(patsubst $(srcdir)/%.c,$(builddir)/$(objdir)/%.c.o, $(srcfiles))

INC_dirs:=$(addprefix -I,$(srcdir))
CFlags:=-std=c17 -Werror -Wall -pedantic $(INC_dirs)
LDFlags:=

all: app.exe

.PHONY: run clean dir

run:
	./app.exe run temp

clean:
	del $(builddir)\$(objdir)\*.o

dir:
	mkdir $(builddir)\$(objdir)

$(builddir)/$(objdir)/%.c.o: $(srcdir)/%.c Makefile
	$(CC) $(CFlags) -c $< -o $@

app.exe: Makefile $(objfiles)
	$(CC) $(objfiles) -o $@ $(LDFlags)
