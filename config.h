#ifndef _CONFIG_H_

#define true 1
#define false 0
typedef int bool;

#define DEFAULT_NR_PAGES	32
//#define NR_ORDERS	12

extern bool verbose;

#define PRINTF(...) \
	if (verbose) printf(__VA_ARGS__)
#endif
