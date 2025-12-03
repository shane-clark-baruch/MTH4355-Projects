CC=clang
CFLAGS=-Wall -Wextra -std=gnu11 -Iinclude

SRC=src/shell.c src/parser.c src/exec.c src/jobs.c
OBJ=$(SRC)

all: shell

shell: $(SRC)
	$(CC) $(CFLAGS) -o shell $(SRC)

clean:
	rm -f shell
