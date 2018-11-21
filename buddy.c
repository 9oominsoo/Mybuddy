/**********************************************************************
 * Copyright (c) 2018
 *	Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/
#include <stdio.h>
#include <errno.h>

#include "config.h"


/**
 * Skeleton data structures to implement the buddy system allocator
 */

/**
 * Data structure to represent an order-@order pages. To the rest of this file,
 * consecutive pages will be represented in @start:@order notation.
 * E.g., 16:3 is 8(2^3)  consecutive pages (or say order-3 page) starting from
 * page frame 16.
 */
struct chunk {
	/**
	 * TODO: Modify this structure as you need.
	 */
	unsigned int start;
	unsigned int order;
};


/**
 * Data structure to maintain order-@order free chunks.
 */
struct chunk_list {
	/**
	 * TODO: Modify this structure as you need
	 */
	unsigned int order;
};


/**
 * Data structure to realize the buddy system allocator
 */
struct buddy {
	/**
	 * TODO: Modify this example data structure as you need
	 */

	/**
	 * Free chunk list in the buddy system allocator.
	 * @NR_ORDERS will be set during compilation (See Makefile for its setting
	 * (default = 12). Also, MAKE SURE your buddy implementation can handle
	 * up to order-(@NR_ORDERS-1).
	 */
	struct chunk_list chunks[NR_ORDERS];

	unsigned int allocated;	/* Number of pages that are allocated */
	unsigned int free;		/* Number of pages that are free */
};


/**
 * This is your buddy system allocator instance!
 */
static struct buddy buddy;


/**
 *    Your buddy system allocator should manage from order-0 to
 *  order-(@NR_ORDERS-1). In the following example, assume your buddy system
 *  manages page 0 to 0x1F (0 -- 31, thus @nr_pages is 32) and pages from
 *  20 to 23 and 28 (0x14 - 0x17, 0x1C) are allocated by alloc_pages()
 *  by some orders.
 *  At this moment, the buddy system will split the address space into;
 *
 *      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
 * 0x00 <-------------------4-------------------------->
 * 0x10 <----2----->X  X  X  X  <-----2---->X  <0><-1-->
 *                  ^  ^  ^  ^              ^
 *                  |--|--|--|--------------|
 *                     allocated
 *
 *   Thus, one of possible buddy systems may maintain free chunk list like;
 *
 * Order     | Start addresses of free chunks
 * ----------------------------------------------
 * NR_ORDERS-1 |
 *    ...    |
 *     4     | 0x00
 *     3     |
 *     2     | 0x10, 0x18
 *     1     | 0x1e
 *     0     | 0x1d
 */


/**
 * Allocate 2^@order contiguous pages.
 *
 * Description:
 *    For example, when @order=0, allocate a single page, @order=2 means
 *  to allocate 4 consecutive pages, and so forth.
 *    From the example state above, alloc_pages(2) will give 0x10 (or 0x18)
 *  through @*page, and the corresponding entry will be removed from the
 *  chunk list. To hanle alloc_pages(3), the order-4 chunk (0x00 -- 0x0f)
 *  should be broken into smaller chunks (say 0x00 -- 0x07, 0x08 -- 0x0f),
 *  and one of the buddies will be returned through @page whereas the other
 *  buddy will be put into order-3 chunk list.
 *
 * Return:
 *   0      : On successful allocation. @*page will contain the starting
 *            page number of the allocated chunk
 *  -EINVAL : When @order < 0 or @order >= NR_ORDERS
 *  -ENOMEM : When order-@order contiguous chunk is not available in the system
 */
int alloc_pages(unsigned int *page, const unsigned int order)
{
	/**
	 * Your implementation will look (but not limited to) like;
	 *
	 * Check whether a chunk is available from chunk_list of @order
	 * if (exist) {
	 *    allocate the chunk from the list; Done!
	 * } else {
	 *    Make a order-@order chunk by breaking a higher-order chunk(s)
	 *    - Find the smallest free chunk that can satisfy the request
	 *    - Break the chunk until it is small enough
	 *    - Put remainders into the free chunk list
	 *
	 *    Return the allocated chunk via @*page
	 *
	 *    Make sure that your implementation handled the cascading case
	 *    where a free chunk is only available from many-times-higher-order
	 *    chunk list.
	 * }
	 *
	 *----------------------------------------------------------------------
	 * Print out below message using PRINTF upon each events. Note it is
	 * possible for multiple events to be happened to handle a single
	 * alloc_pages(). Also, MAKE SURE TO USE PRINTF, _NOT_ printf.
	 *
	 * - Split an order-@x chunk starting from @page into @left and @right:
	 *   PRINTF("SPLIT 0x%x:%u -> 0x%x:%u + 0x%x:%u\n",
	 *			page, x, left, x-1, right, x-1);
	 *
	 * - Put an order-@x chunk starting from @page into the free list:
	 *   PRINTF("PUT   0x%x:%u\n", page, x);
	 *
	 * - Allocate an order-@x chunk starting from @page for serving the request:
	 *   PRINTF("ALLOC 0x%x:%x\n", page, x);
	 *
	 * Example: A order-4 chunk starting from 0 is split into 0:3 and 8:3,
	 * and 0:3 is split again to 0:2 and 4:2 to serve an order-2 allocation.
	 * And then 0:2 is allocated:
	 *
	 * SPLIT 0x0:4 -> 0x0:3 + 0x8:3
	 * PUT   0x8:3
	 * SPLIT 0x0:3 -> 0x0:2 + 0x4:2
	 * PUT   0x4:2
	 * ALLOC 0x0:2
	 *
	 *       OR
	 *
	 * SPLIT 0x0:4 -> 0x0:3 + 0x8:3
	 * SPLIT 0x0:3 -> 0x0:2 + 0x4:2
	 * PUT   0x8:3
	 * PUT   0x4:2
	 * ALLOC 0x0:2
	 *
	 * ALL POSSIBLE AND CORRECT COMBINATIONS WILL BE ACCEPTED
	 *----------------------------------------------------------------------
	 */

	buddy.allocated += (1 << order);
	buddy.free -= (1 << order);
	return 0;
}


/**
 * Free @page which are contiguous for 2^@order pages
 *
 * Description:
 *    Assume @page was allocated by alloc_pages(@order) above. Don't forget
 *  to merge freed chunk with its buddy if it exists.
 */
void free_pages(unsigned int page, const unsigned int order)
{
	/**
	 * Your implementation will look (but not limited to) like;
	 *
	 * Find the buddy chunk from this @order.
	 * if (buddy does not exist in this order-@order free list) {
	 *    put into this chunk list. Problem solved!!!
	 * } else {
	 *    Merge with the buddy
	 *    Promote the merged chunk into the higher-order chunk list
	 *
	 *    Consider the cascading case as well; in the higher-order list, there
	 *    might exist its buddy again, and again, again, ....
	 * }
	 *
	 *----------------------------------------------------------------------
	 * Similar to alloc_pages, print following messages when the event happens;
	 *
	 * - Merge order-$x buddies starting from $left and $right:
	 *   PRINTF("MERGE : 0x%x:%u + 0x%x:%u -> 0x%x:%u\n",
	 *			left, x, right, x, left, x+1);
	 *
	 * - Put an order-@x chunk starting from @page into the free list:
	 *   PRINTF("PUT  : 0x%x:%u\n", page, x);
	 *
	 * Example: Two buddies 0:2 and 4:2 (0:2 indicates an order-2 chunk
	 * starting from 0) are merged to 0:3, and it is merged again with 8:3,
	 * producing 0:4. And then finally the chunk is put into the order-4 free
	 * chunk list:
	 *
	 * MERGE : 0x0:2 + 0x4:2 -> 0x0:3
	 * MERGE : 0x0:3 + 0x8:3 -> 0x0:4
	 * PUT   : 0x0:4
	 *----------------------------------------------------------------------
	 */
	buddy.allocated -= (1 << order);
	buddy.free += (1 << order);
}


/**
 * Print out the order-@order free chunk list
 *
 *  In the example above, print_free_pages(0) will print out:
 *  0x1d:0
 *
 *  print_free_pages(2):
 *    0x10:2
 *    0x18:2
 */
void print_free_pages(const unsigned int order)
{
	unsigned int starting_page = 0x43; /* I love 43 because it's perfect!! */

	/**
	 * Your implementation should print out each free chunk
	 * in the following format;
	 */
	printf("    0x%x:%u\n", starting_page, order);
}


/**
 * Print out the unusable index(UI) for order-@order.
 *
 * Description:
 *    Return the unusable index of @order. In the above example, we have 27 free
 *  pages;
 *  27 = sum(i = 0 to @NR_ORDERS-1){ (1 << i) * # of order-i free chunks }
 *    and
 *  UI(0) = 0 / 27 = 0.0 (UI of 0 is always 0 in fact).
 *  UI(1) = 1 (for 0x1d) / 27 = 0.037
 *  UI(2) = (1 (0x1d) + 2 (0x1e-0x1f)) / 27 = 0.111
 *  UI(3) = (1 (0x1d) + 2 (0x1e-0x1f) + 4 (0x10-0x13) + 4 (0x18-0x1b)) / 27
 *        = 0.407
 *  ...
 */
double get_unusable_index(unsigned int order)
{
	return 0.0;
}


/**
 * Initialize your buddy system.
 *
 * @nr_pages_in_order: number of pages in order-n notation to manage.
 * For instance, if @nr_pages_in_order = 8, the system should be able to
 * manage 256 pages
 *
 * Return:
 *   0      : On successful initialization
 *  -EINVAL : Invalid arguments or when something goes wrong
 */
int init_buddy(unsigned int nr_pages_in_order)
{
	int i;

	buddy.allocated = 0;
	buddy.free = 1 << nr_pages_in_order;

	/* TODO: Do your initialization as you need */

	for (i = 0; i < NR_ORDERS; i++) {
		buddy.chunks[i].order = i;
	}

	/* TODO: Don't forget to initiate the free chunk list with
	 * order-(@NR_ORDERS-1) chunks. Note you may add multiple chunks if
	 * @nr_pages_in_order >= @NR_ORDERS. For instance, when
	 * @nr_pages_in_order=10 and @NR_ORDERS=10, the initial free chunk
	 * list will have two chunks; 0x0:9, 0x200:9.
	 */

	return 0;
}


/**
 * Return resources that your buddy system is using. No other function will
 * be called after calling this function.
 */
void fini_buddy(void)
{
	/* TODO: Do your finalization if needed */
}
