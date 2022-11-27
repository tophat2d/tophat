// include umka api for UmkaStackSlot.
#include <umka_api.h>
#include <stdio.h>
#define THEXT
#include "../../src/tophat.h"

/* Funcation names are automatically matched to function prototyppes in c.
 * Be aware that this might produce unexpected results with colliding fucntions.
 * It's a good practice to prefix the c functions.
 *
 * Compile with: cc -fPIC -shared -o main.umi main.c \
 *   -I../../lib/umka/src -I../../lib/miniaudio -I../../lib/stb
 *
 * p is an array of parameters, indexed from the last.
 * r is a pointer to the return value.
 * See more about how to make bindings in umka's docs.
 */

th_global *thg = NULL;

void init(UmkaStackSlot *p, UmkaStackSlot *r) {
	thg = (th_global *)p[1].ptrVal;
	th_ext_set((void **)p[0].ptrVal);
}

void add(UmkaStackSlot *p, UmkaStackSlot *r) {
	int a = p[1].intVal;
	int b = p[0].intVal;

	r->intVal = a + b;
}

void printHello(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_canvas_text("hello from extension in c", 0xff, (th_vf2){{0, 15}}, 1);
}
