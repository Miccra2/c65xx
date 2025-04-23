CC = /usr/bin/cc
CCFLAGS = -Wall -Wextra -ggdb -I./include -c
LDFLAGS = -lc

all: clean mkdir e65.o

a65.o:
	$(CC) $(CCFLAGS) -o build/a65.o src/a65.c

e65.o:
	$(CC) $(CCFLAGS) -o build/e65.o src/e65.c

d65.o:
	$(CC) $(CCFLAGS) -o build/d65.o src/d65.c

a65cli.o:
	$(CC) $(CCFLAGS) -o build/a65cli.o src/a65cli.c

e65cli.o:
	$(CC) $(CCFLAGS) -o build/e65cli.o src/e65cli.c

d65cli.o:
	$(CC) $(CCFLAGS) -o build/d65cli.o src/d65cli.c


a65: a65.o a65cli.o
	$(CC) -L $(LDFLAGS) -o a65 build/a65.o build/a65cli.o

e65: e65.o e65cli.o
	$(CC) -L $(LDFLAGS) -o e65 build/e65.o build/e65cli.o

d65: d65.o d65cli.o
	$(CC) -L $(LDFLAGS) -o d65 build/d65.o build/d65cli.o

clean:
	rm -rf build

mkdir:
	mkdir -p build
