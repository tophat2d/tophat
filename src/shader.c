#include <string.h>
#include <stdlib.h>

#include <GL/gl.h>
#include "tophat.h"

extern th_global thg;

int th_shader_compile(
	char *vertf, char *fragf, char *vertb, char *fragb,
	const char **verta, int vertac) {

	char *vert = calloc(1, strlen(vertf) + strlen(vertb) + 1);
	if (!vert)
		return 1;
	strcpy(vert, vertb);
	strcat(vert, vertf);

	char *frag = calloc(1, strlen(fragf) + strlen(fragb) + 1);
	if (!frag)
		return 1;
	strcpy(frag, fragb);
	strcat(frag, fragf);

	printf("%s\n%s\n\n\n\n", vert, frag);
	GLuint prog = th_gl_create_prog(vert, frag, verta, vertac);
	th_shader *s = th_alloc_shader();
	*s = prog;

	return thg.shader_count;
}
