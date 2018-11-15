#ifndef _CHECKER_H_

int mark_alloc_pages(const unsigned int page, const unsigned int order);
int clear_alloc_pages(const unsigned int id);
void list_alloc_pages(void);

int init_checker(void);
void fini_checker(void);
#endif
