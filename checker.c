#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <sys/queue.h>

#include "config.h"

static unsigned int __id = 0;
struct op {
	unsigned int id;	/* Alloc operation id */
	unsigned int order;
	unsigned int page;
	TAILQ_ENTRY(op) list;
};
static TAILQ_HEAD(tailhead, op) ops;

int mark_alloc_pages(const unsigned int page, const unsigned int order)
{
	struct op *op = NULL;


	TAILQ_FOREACH(op, &ops, list) {
		if (op->page <= page && page < op->page + (1 << op->order)) {
			fprintf(stderr, "Page %u was allocated by operation id=%u\n",
					page, op->id);
			return -EINVAL;
		}
	}

	op = malloc(sizeof(*op));
	assert(op);

	op->id = __id++;
	op->page = page;
	op->order = order;
	TAILQ_INSERT_TAIL(&ops, op, list);

	PRINTF(" Allocate order-%u page(s) from %u (id=%u)\n", order, page, op->id);
	return 0;
}

int clear_alloc_pages(const unsigned int id)
{
	struct op *op;

	TAILQ_FOREACH(op, &ops, list) {
		if (id == op->id) {
			TAILQ_REMOVE(&ops, op, list);
			PRINTF(" Free order-%u page(s) from %u (id=%u)\n",
					op->order, op->page, op->id);

			free(op);
			return 0;
		}
	}

	fprintf(stderr, "No alloc operation with id=%u\n", id);
	return -ENOENT;
}

void list_alloc_pages(void)
{
	struct op *op;

	printf("%6s  %8s  %2s\n", "ID", "page", "order");
	printf("-----------------------\n");
	TAILQ_FOREACH(op, &ops, list) {
		printf("%6u: %8u  %2u\n", op->id, op->page, op->order);
	}
}

int init_checker(void)
{
	TAILQ_INIT(&ops);
	return 0;
}

void fini_checker(void)
{
	struct op *op;
	while (!TAILQ_EMPTY(&ops)) {
		op = TAILQ_FIRST(&ops);
		TAILQ_REMOVE(&ops, op, list);
		free(op);
	}
}
