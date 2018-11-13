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
	TAILQ_ENTRY(op) next;
};
static TAILQ_HEAD(tailhead, op) ops;

int mark_alloc_pages(const unsigned int page, const unsigned int order)
{
	struct op *op = NULL;

	for (op = ops.tqh_first; op != NULL; op = op->next.tqe_next) {
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
	TAILQ_INSERT_TAIL(&ops, op, next);

	PRINTF("Alloc order-%u page(s) from %u (id=%u)\n", order, page, op->id);
	return 0;
}

int clear_alloc_pages(const unsigned int id)
{
	struct op *op;

	for (op = ops.tqh_first; op != NULL; op = op->next.tqe_next) {
		if (id == op->id) {
			TAILQ_REMOVE(&ops, op, next);
			PRINTF("Free order-%u page(s) from %u (id=%u)\n",
					op->order, op->page, op->id);

			free(op);
			return 0;
		}
	}

	fprintf(stderr, "No alloc operation with id=%u\n", id);
	return -ENOENT;
}

int init_checker(void)
{
	TAILQ_INIT(&ops);
	return 0;
}

void fini_checker(void)
{
	struct op *op;
	while (ops.tqh_first) {
		op = ops.tqh_first;
		TAILQ_REMOVE(&ops, op, next);
		free(op);
	}
}
