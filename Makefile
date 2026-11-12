CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

mmu: mmu.c list.c util.c list.h util.h
	$(CC) $(CFLAGS) -o mmu mmu.c list.c util.c

clean:
	rm -f mmu
