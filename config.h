#ifndef _CONFIG_H_

#define true 1
#define false 0
typedef int bool;

#define DEFAULT_NR_PAGES_IN_ORDER	12
//#define NR_ORDERS	12

extern bool verbose;

#define PRINTF(...) \
	if (verbose > 0) printf(__VA_ARGS__)
#define LOG(...) \
	if (verbose > 1) printf(__VA_ARGS__)
#endif
