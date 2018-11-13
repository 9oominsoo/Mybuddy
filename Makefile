CC=gcc
CFLAGS=-g
LDFLAGS=-lpthread

HEADERS=$(wildcard ./*.h)

.PHONY: all
all: pa4

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

pa4: main.o buddy.o checker.o
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf *.o pa4
