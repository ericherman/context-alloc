#ifndef MALLOC_WHINE_H
#define MALLOC_WHINE_H 1

#ifndef NDEBUG
#ifndef Malloc_whine
#include <stdio.h>
#define Malloc_whine(log, size) do { \
	fprintf(log, "%s:%d alloc returned NULL for %zu bytes?\n", \
		 __FILE__, __LINE__, size); \
	} while (0)
#endif
#endif

#ifndef Malloc_whine
#define Do_Nothing do { ((void)0); } while (0)
#define Malloc_whine(log, size) Do_Nothing
#endif

#endif /* MALLOC_WHINE_H */
