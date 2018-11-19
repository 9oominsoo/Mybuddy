NR_ORDERS=12
CC=gcc
CFLAGS=-g -Wall -DNR_ORDERS=$(NR_ORDERS)
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
	rm -rf *.o pa4 cscope.out

.PHONY: test0
test0: clean pa4 workloads/test0
	./pa4 -q -n 12 < workloads/test0 > RESULT

.PHONY: test1
test1: clean pa4 workloads/test1
	./pa4 -q -n 12 < workloads/test1 > RESULT

.PHONY: cscope
cscope:
	cscope -b -R
