COMPILER = gcc

all: header program

program: shell.c head
	$(COMPILER) -g -o shell shell.c

header: shell.h
	$(COMPILER) -g -o head shell.h

run: shell
	./shell

debug: shell
	gdb shell

clean:
	rm -f shell
	rm -f head
