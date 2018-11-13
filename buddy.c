#include <stdio.h>
#include <errno.h>

static unsigned int nr_pages = 0;
static unsigned int offset = 0;

/**
 * Allocate 2^order contiguous pages.
 * 
 * Description:
 *  For example, when @order=0, allocate a single page, @order=2 means
 * to allocate 4 consecutive pages.
 *
 * Return:
 *  0:       On successful allocation. *page contains the first page id
 *  -EINVAL: When order < 0 or order > 10
 *  -ENOMEM: When there is no such a contiguous region in the system.
 */
int alloc_pages(unsigned int *page, const unsigned int order)
{
	unsigned int alloc = offset;
	offset += (1 << order);
	*page = alloc;
	return 0;
}


/**
 * Free @page which are contiguous for 2^@order pages
 *
 * Description:
 *  Assume @page was allocated by alloc_pages(@order) above.
 *  Bonus point will be given if you can detect a malicious free_pages().
 *
 * Return:
 *  0       : On successful deallocation
 *  -EINVAL : On a malicious free_pages (optional)
 */
int free_pages(unsigned int page, const unsigned int order)
{
	return -EINVAL;
}


/**
 * Print out the list of free pages
 *
 * Description:
 *  Assume your buddy manages page 0 to 30, and page 20, 21, 22, 23, and 28
 * are allocated by alloc_pages(). In this case, the output of this function
 * should be like;
 *
 *   0 20  // There are 20 contiguous pages starting from 0 (0-19)
 *   24 4  // There are 4 contiguous pages starting from 24
 *   29 2  // There are two consecutive pages starting from 29
 */
void print_free_pages(void)
{
}


/**
 * Print out the list of free pages that are contiguous for at least 2^@order
 *
 * Description:
 *  Assume the case described above. print_free_contiguous_pages(2) will print
 * out the memory chunks that are contiguous for 4 pages. Thus, output should
 * be;
 *
 *   0 20 // This chunk is longer than or equal to 2^2
 *   24 4 // This chunk is 2^2 in size.
 *        // 29 2 SHOULD NOT be printed since the chunk is smaller than 2^2)
 *
 *  Effectively, print_free_pages() is equivalent to
 * print_free_contiguous_pages(0).
 */
void print_free_contiguous_pages(unsigned int order)
{
}


/**
 * Print out the current unusable index for @order.
 *
 * Description:
 *  Print the unusable index of @order.
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
 *  0       : On successful initialization
 *  -EINVAL : Invalid arguments or something went south
 */
int init_buddy(unsigned int _nr_pages_)
{
	nr_pages = _nr_pages_;
	return 0;
}


/**
 * Return resources that your buddy system is using. No other function will 
 * be called after calling this function.
 */
void fini_buddy(void)
{
}
