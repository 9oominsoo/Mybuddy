#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

#include "config.h"
#include "buddy.h"
#include "checker.h"

bool verbose = false;
static unsigned int nr_pages = DEFAULT_NR_PAGES;

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
		fprintf(stderr, "Wrong %s operation (argc %d != %d)\n",
				name, expect, argc);
		return true;
	}
	return false;
}

/**
 * Process CLI inputs
 */
static void __process_operations(void)
{
	bool keep_running = true;
	int argc;
	char *argv[60] = {NULL};
	char line[120];
	unsigned int order;
	unsigned int page;

	while (keep_running && printf(">> ") && fgets(line, sizeof(line), stdin)) {
		argc = __parse_operation(line, argv);
		if (argc <= 0) {
			break;
		}
		if (strlen(argv[0]) != 1) {
			printf("Unknown operation '%s'\n", argv[0]);
			break;
		}
		switch(tolower(argv[0][0])) {
		case '#': /* comment */
			continue;
		case 'a': /* allocation */
			if (__check_operation(2, argc, "alloc")) continue;
			order = atoi(argv[1]);
			if (alloc_pages(&page, order) == 0) {
				mark_alloc_pages(page, order);
			}
			break;
		case 'd': /* free / deallocation */
		case 'f':
			if (__check_operation(2, argc, "free")) continue;
			clear_alloc_pages(atoi(argv[1]));
			break;
		case 'p': /* print list */
			if (__check_operation(2, argc, "print")) continue;
			print_free_pages(atoi(argv[1]));
			break;
		case 'x': /* dump all lists */
			if (__check_operation(1, argc, "print all")) continue;
			for (order = 0; order < NR_ORDERS; order++) {
				print_free_pages(order);
			}
			break;
		case 'l': /* list operations */
			if (__check_operation(1, argc, "list")) continue;
			list_alloc_pages();
			break;
		case 'u': /* check unusable index */
			if (__check_operation(2, argc, "unusable index")) continue;
			order = atoi(argv[1]);
			PRINTF("Unusable index for order %d is ", order);
			printf("%.4f\n", get_unusable_index(order));
			break;
		case 'h': /* Help!! */
			printf("  a <order> : allocate a 2^@order chunk\n");
			printf("  d <id>    : free the chunk whose allocation id is @id\n");
			printf("  f <id>    : same to 'd'\n");
			printf("  p <order> : print out the free chunks of @order\n");
			printf("  x         : print all free chunks\n");
			printf("  l         : show the allocated state\n");
			break;
		case 'q': /* I'm done. Good bye*/
			keep_running = false;
			break;
		default:
			fprintf(stderr, "Unknown operation '%c'\n", argv[0][0]);
		}
	}
}


static int __parse_options(int argc, char *argv[]) {
	char opt;
	while ((opt = getopt(argc, argv, "h?vqn:a:")) != -1) {
		switch(opt) {
		case 'n': /* set the number of pages to manage */
			nr_pages = atol(optarg);
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
			fprintf(stderr, "  -v : verbose\n");
			fprintf(stderr, "  -q : be quiet\n");
			fprintf(stderr, "  -n <nr_pages>\n");

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

	PRINTF("Starting the buddy system with %u pages. Have fun!!\n", nr_pages);

	if (init_checker()) return EXIT_FAILURE;

	if (init_buddy(nr_pages))
		goto out_checker;

	__process_operations();

	fini_buddy();

out_checker:
	fini_checker();

	return EXIT_SUCCESS;
}
