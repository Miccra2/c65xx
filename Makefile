CC = /usr/bin/cc
CCFLAGS = -Wall -Wextra -ggdb

all: clean asm65 emu65

asm65:
	$(CC) $(CCFALGS) -o asm65 src/asm65.c

emu65:
	$(CC) $(CCFLAGS) -o emu65 src/emu65.c

clean:
	rm -rf asm65 emu65
