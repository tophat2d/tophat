// include umka api for UmkaStackSlot.
#include "../../lib/umka/src/umka_api.h"
#include <stdio.h>
#include "../../src/ext/thext.h"

/* Funcation names are automatically matched to function prototyppes in c.
 * Be aware that this might produce unexpected results with colliding fucntions.
 * It's a good practice to prefix the c functions.
 *
 * Compile with: cc -shared -o main.umi main.c
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
