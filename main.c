#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "buddy.h"
#include "checker.h"

bool verbose = true;
static unsigned int nr_pages = DEFAULT_NR_PAGES;
static unsigned int nr_allocators = DEFAULT_NR_ALLOCATORS;

static int __parse_operation(char *line, char *argv[])
{
	int argc = 0;
	char *c = line;
	char *start;
	bool str = false;

	while (c && *c != '\r' && *c != '\n') {
		if (*c == '\t' || *c == ' ') {
			*c = '\0';
			c++;
			if (str) argv[argc++] = start;
			str = false;
			continue;
		}

		if (!str) {
			start = c;
		}
		str = true;
		c++;
	}
	if (c && (*c == '\r' || *c == '\n') && str) {
		*c = '\0';
		argv[argc++] = start;
	}
	return argc;
}

static bool __check_operation(const int expect, const int argc, const char *name)
{
	if (expect != argc) {
		fprintf(stderr,
				"Wrong %s operation (argc %d != %d)\n", name, expect, argc);
		return true;
	}
	return false;
}

static void __process_operations(void)
{
	bool keep_running = true;
	int argc;
	char *argv[60] = {NULL};
	char line[120];
	unsigned int page;

	PRINTF("$ ");

	while (keep_running && fgets(line, sizeof(line), stdin)) {
		argc = __parse_operation(line, argv);
		if (argc <= 0) {
			break;
		}
		if (strlen(argv[0]) != 1) {
			printf("Unknown operation '%c'\n", argv[0]);
			break;
		}
		switch(tolower(argv[0][0])) {
		case 'a':
			if (__check_operation(2, argc, "alloc")) {
				continue;
			}
			if (alloc_pages(&page, atoi(argv[1])) == 0) {
				mark_alloc_pages(page, atoi(argv[1]));
			}
			break;
		case 'd':
		case 'f':
			if (__check_operation(2, argc, "free")) {
				continue;
			}
			clear_alloc_pages(atoi(argv[1]));
			break;
		case 'p':
			if (__check_operation(2, argc, "print")) {
				continue;
			}
			break;
		case 'x':
			if (__check_operation(1, argc, "print all")) {
				continue;
			}
			break;
		case 'l':
			if (__check_operation(1, argc, "list")) {
				continue;
			}
			list_alloc_pages();
			break;
		case 'q':
		default:
			keep_running = false;
			break;
		}
		PRINTF("$ ");
	}
}


static int __parse_options(int argc, char *argv[]) {
	char opt;
	while ((opt = getopt(argc, argv, "h?vqn:a:")) != -1) {
		switch(opt) {
		case 'n':
			nr_pages = atol(optarg);
			break;
		case 'a':
			nr_allocators = atoi(optarg);
			break;
		case 'v':
			verbose = true;
			break;
		case 'q':
			verbose = false;
			break;
		case 'h':
		case '?':
		default:
			fprintf(stderr, "Usage: %s\n", argv[0]);
			return -EBUSY;
			break;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	if (__parse_options(argc, argv))
		return EXIT_FAILURE;

	// PRINTF("- Allocators : %d\n", nr_allocators);
	PRINTF("- Pages      : %u\n", nr_pages);

	if (init_checker()) return EXIT_FAILURE;

	if (init_buddy(nr_pages))
		goto out_checker;

	__process_operations();

	fini_buddy();

out_checker:
	fini_checker();

	return EXIT_SUCCESS;
}
