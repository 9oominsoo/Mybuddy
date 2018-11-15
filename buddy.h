#ifndef __BUDDY_H__
#define __BUDDY_H__

/* Interface functions that the framework will call */
int alloc_pages(unsigned int *page, unsigned int order);
int free_pages(unsigned int page, unsigned int order);

double get_unusable_index(unsigned int order);
void print_free_pages(const unsigned int order);

/* Will be invoked when it is the time to initialize your buddy system */
int init_buddy(unsigned int nr_pages);

/* Will be invoked when to release resources that you allocated */
void fini_buddy(void);
#endif
