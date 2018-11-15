#include <stdio.h>
#include <errno.h>

#include "config.h"

/**
 * @nr_pages: the number of pages that your buddy system should maintain.
 *
 * This value will be set through init_buddy(), and once it is set, it will
 * not be changed thereafter.
 */
static unsigned int nr_pages = 0;


/** 
 *  XXX: Sample implementation which does not work correctly!!
 *
 *  Change/delete/remove/comment out, or do whatever you want to do
 */
static unsigned int offset = 0;


/**
 * Skeleton data structures to implement the buddy system allocator
 *
 *  struct chunk_list is supposed to be used to maintain order-@order
 * free chunks.
 */
struct chunk_list {
	unsigned int order;
	/**
	 * You may (or may not) need a list of chunks in this @order.
	 */
};

struct buddy {
	struct chunk_list chunks[NR_ORDERS];
	unsigned int allocated;	/* Number of pages that are allocated */
	unsigned int free;		/* Number of pages that are free */
};

struct buddy buddy;


/**
 *    Your buddy system allocator should manage from order-0 to
 *  order-(@NR_ORDERS-1). In the following example, assume your buddy system
 *  manages page 0 to 0x1F (0 -- 31, thus @nr_pages is 32) and pages from
 *  20 to 23 and 28 (0x14 - 0x17, 0x1C) are allocated by alloc_pages()
 *  by some orders.
 *  At this moment, the buddy system will split the address space like;
 *
 *      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
 * 0x00 <-------------------4-------------------------->
 * 0x10 <----2----->X  X  X  X  <-----2---->X  <0><-1-->
 *
 *   Thus, the buddy system may maintain free chunk list like;
 *
 * Order     | Start addresses of free chunks
 * ----------------------------------------------
 * NR_ORDERS-1 |
 *  ...
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
	unsigned int alloc = offset;
	offset += (1 << order);
	*page = alloc;

	/**
	 * Your implementation will look (but not limited to) like;
	 *
	 * Check whether a chunk is available from chunk_list of @order
	 * if (exist) {
	 *    allocate the chunk from the list; Done!
	 * } else {
	 *    Make a order-@order chunk by breaking a higher-order chunk(s).
	 *    Return the allocated chunk via @*page
	 *    Put the remainders into corresponding chunk lists
	 *    
	 *    Make sure that your implementation handled the cascading case
	 *    where a free chunk is only available from many-times-higher-order
	 *    chunk list.
	 * }
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
 *
 * Return:
 *   0      : On successful deallocation
 *  -EINVAL : On a malicious free_pages (bonus points!)
 */
int free_pages(unsigned int page, const unsigned int order)
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
	 */
	buddy.allocated -= (1 << order);
	buddy.free += (1 << order);
	return -EINVAL;
}


/**
 * Print out the order-@order free chunk list
 *
 *  In the example above, print_free_pages(0) will print out:
 *  0x1d 0
 *
 *  print_free_pages(2):
 *  0x10 2
 *  0x18 2
 */
void print_free_pages(const unsigned int order)
{
	unsigned int starting_page = 0x43; /* I love 43 because it's perfect!! */

	/**
	 * Your implementation should print out each free chunk
	 * in the following format;
	 */
	printf("0x%x %u\n", starting_page, order);
}


/**
 * Print out the unusable index(UI) for order-@order.
 *
 * Description:
 *    Print the unusable index of @order. In the above example, we have 27 free
 *  pages;
 *  27 = sum(i = 0 to @NR_ORDERS-1){ (1 << i) x # of free chunks of order-i }
 *    and
 *  UI(0) = 0 / 27 = 0.0 (UI of 0 is always 0 in fact).
 *  UI(1) = 1 (for 0x1d) / 27 = 0.037
 *  UI(2) = (1 (0x1d) + 2 (0x1e-0x1f)) / 27 = 0.111
 *  UI(3) = (1 (0x1d) + 2 (0x1e-0x1f) + 4 (0x10-0x13) + 4 (0x18-0x1b)) / 27
 *        = 0.407
 *  ...
 *
 */
double get_unusable_index(unsigned int order)
{
	return 0.0;
}


/**
 * Initialize your buddy system.
 *
 * @nr_pages: number of pages to manage. it will be between 1 to 2 << 16
 *
 * Return:
 *   0      : On successful initialization
 *  -EINVAL : Invalid arguments or when something goes wrong
 */
int init_buddy(unsigned int _nr_pages_)
{
	int i;
	nr_pages = _nr_pages_;

	buddy.allocated = 0;
	buddy.free = nr_pages;

	for (i = 0; i < NR_ORDERS; i++) {
		buddy.chunks[i].order = i;
	}

	return 0;
}


/**
 * Return resources that your buddy system is using. No other function will 
 * be called after calling this function.
 */
void fini_buddy(void)
{
}
