#include "tophat.h"
#include <stdlib.h>
#include <string.h>

extern th_global thg;

#define OPAQUE(name, type) \
\
type *th_get_##name(uu index) {\
	if (!index) return NULL; \
	if (index > thg.name##_count) \
		return NULL; \
	return &thg.name##s[index-1]; \
} \
\
type *th_get_##name##_err(uu index) {\
	type *p = th_get_##name(index); \
	if (!p) \
		th_error("Could not find " #name " with index %d.", index); \
	return p; \
} \
\
type *th_alloc_##name() { \
	void *p = realloc(thg.name##s, (++thg.name##_count + 1) * sizeof(type)); \
	if (!p) \
		return NULL; \
	thg.name##s = p; \
	memset(&thg.name##s[thg.name##_count - 1], 0, sizeof(type)); \
	return &thg.name##s[thg.name##_count - 1]; \
} \


OPAQUE(font, th_font)
OPAQUE(shader, th_shader)
