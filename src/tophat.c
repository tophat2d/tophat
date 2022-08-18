#include "tophat.h"
#include <stdlib.h>
#include <string.h>

extern th_global thg;

th_shader *th_get_shader(uu index) {
	if (!index) return NULL;
	if (index > thg.shader_count)
		return NULL;
	return &thg.shaders[index-1];
}

th_shader *th_get_shader_err(uu index) {
	th_shader *p = th_get_shader(index);
	if (!p)
		th_error("Could not find shader with index %d.", index);
	return p;
}

th_shader *th_alloc_shader() { 
	void *p = realloc(thg.shaders, (++thg.shader_count + 1) * sizeof(th_shader));
	if (!p)
		return NULL;
	thg.shaders = p;
	memset(&thg.shaders[thg.shader_count - 1], 0, sizeof(th_shader));
	return &thg.shaders[thg.shader_count - 1];
}
