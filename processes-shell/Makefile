CC=clang
CFLAGS=-Wall -Wextra -std=gnu11

all: shell

shell: src/shell.c
	$(CC) $(CFLAGS) -o shell src/shell.c

clean:
	rm -f shell
