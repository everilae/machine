#ifndef DEBUG_H
#define DEBUG_H 1

#include <stdio.h>
#include <time.h>

#ifdef DEBUG

# define PDEBUG(...) do { \
	struct timespec ts; \
	clock_gettime(CLOCK_MONOTONIC, &ts); \
	fprintf(stderr, "[%li.%09li] ", ts.tv_sec, ts.tv_nsec); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
} while (0)

# define PDEBUGIF(predicate, ...) do { \
	if (predicate) { \
		PDEBUG(__VA_ARGS__); \
	} \
} while (0)

#else

# define PDEBUG(...)
# define PDEBUGIF(...)

#endif

#endif
