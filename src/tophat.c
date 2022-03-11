#include "tophat.h"
#include <stdlib.h>
#include <string.h>

extern th_global thg;

#define GET_FUNC(name) th_##name *th_get_##name(uu index) {\
	if (!index) return NULL; \
	if (index > thg.name##_count) \
		return NULL; \
	return &thg.name##s[index-1]; \
}

#define GET_FUNC_ERR(name) th_##name *th_get_##name##_err(uu index) {\
	th_##name *p = th_get_##name(index); \
	if (!p) \
		th_error("Could not find " #name " with index %d.", index); \
	return p; \
}

#define ALLOC_FUNC(name) th_##name *th_alloc_##name() { \
	void *p = realloc(thg.name##s, (++thg.name##_count + 1) * sizeof(th_##name)); \
	if (!p) \
		return NULL; \
	thg.name##s = p; \
	memset(&thg.name##s[thg.name##_count - 1], 0, sizeof(th_##name)); \
	return &thg.name##s[thg.name##_count - 1]; \
}

GET_FUNC(image)
GET_FUNC(font)
GET_FUNC(sound)

GET_FUNC_ERR(image)
GET_FUNC_ERR(font)
GET_FUNC_ERR(sound)

ALLOC_FUNC(image)
ALLOC_FUNC(font)
ALLOC_FUNC(sound)
