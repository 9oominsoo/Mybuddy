#ifndef __BUDDY_H__
#define __BUDDY_H__

int alloc_pages(unsigned int *page, unsigned int order);
int free_pages(unsigned int page, unsigned int order);

double get_unusable_index(unsigned int order);
void print_free_pages(void);

int init_buddy(unsigned int nr_pages);
void fini_buddy(void);

#endif
