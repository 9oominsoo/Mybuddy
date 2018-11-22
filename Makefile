MAX_ORDER=11
CC=gcc
CFLAGS=-g -Wall -DMAX_ORDER=$(MAX_ORDER)
LDFLAGS=-lpthread
VALGRIND=valgrind --leak-check=full --show-leak-kinds=all

HEADERS=$(wildcard ./*.h)

.PHONY: all
all: pa4

%.o: %.c $(HEADERS) Makefile
	$(CC) -c $(CFLAGS) $< -o $@

pa4: main.o buddy.o checker.o
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf *.o pa4 *-result cscope.out

# Allocation test
.PHONY: test-alloc
test-alloc: pa4
	./pa4 -q < workloads/alloc 2> alloc-result
	@diff -wB alloc-result workloads/alloc-ref
	@echo
	@echo "*******************************************************"
	@echo "****  Congraturations, allocation test PASSED!!!!  ****"
	@echo "*******************************************************"

# Free test
.PHONY: test-free
test-free: pa4
	./pa4 -q < workloads/free 2> free-result
	@diff -b free-result workloads/free-ref
	@echo
	@echo "*************************************************"
	@echo "****  Congraturations, free test PASSED!!!!  ****"
	@echo "*************************************************"

# Advanced operations test
.PHONY: test-full
test-full: pa4
	./pa4 -n 13 -q < workloads/full 2> full-result
	@diff -b full-result workloads/full-ref
	@echo
	@echo "*************************************************"
	@echo "****  Congraturations, full test PASSED!!!!  ****"
	@echo "*************************************************"

# Basic leak test
.PHONY: test-leak-basic
test-leak-basic: pa4
	$(VALGRIND) ./pa4 -q < workloads/free

# Full memory leak test
.PHONY: test-leak-full
test-leak-full: pa4
	$(VALGRIND) ./pa4 -n 13 -q < workloads/full

.PHONY: cscope
cscope:
	cscope -b -R
