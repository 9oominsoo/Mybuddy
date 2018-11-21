NR_ORDERS=12
CC=gcc
CFLAGS=-g -Wall -DNR_ORDERS=$(NR_ORDERS)
LDFLAGS=-lpthread

HEADERS=$(wildcard ./*.h)

.PHONY: all
all: pa4

%.o: %.c $(HEADERS) Makefile
	$(CC) -c $(CFLAGS) $< -o $@

pa4: main.o buddy.o checker.o
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf *.o pa4 cscope.out

.PHONY: test0	# Simple test
test0: clean pa4 workloads/test0
	./pa4 -n 12 $(GRADING) < workloads/test0 $(OUTPUT)

.PHONY: test1	# Intermediate test
test1: clean pa4 workloads/test1
	./pa4 -n 12 $(GRADING) < workloads/test1 $(OUTPUT)

.PHONY: test2	# Advanced test
test2: clean pa4 workloads/test1
	./pa4 -n 13 $(GRADING) < workloads/test2 $(OUTPUT)

.PHONY: cscope
cscope:
	cscope -b -R
