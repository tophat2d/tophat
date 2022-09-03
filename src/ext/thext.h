#ifndef THEXT_H
#define THEXT_H
#define THEXT
#include "../tophat.h"
#undef THEXT

#define THEXT(ret, name, ...) static ret (*name)(__VA_ARGS__) = NULL
#include "thextdata.h"
#undef THEXT

static int th_ext_count() {
	int count = 0;
#define THEXT(ret, name, ...) ++count
#include "thextdata.h"
#undef THEXT
	return count;
}

static void th_ext_set(void **arr) {
	int count = th_ext_count();

	int idx = 0;
#define THEXT(ret, name, ...) name = arr[idx++]
#include "thextdata.h"
#undef THEXT
}

#define THEXT

#endif
