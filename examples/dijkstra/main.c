// include umka api for UmkaStackSlot.
#include "../../lib/umka/src/umka_api.h"
#include <stdio.h>

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


void add(UmkaStackSlot *p, UmkaStackSlot *r) {
	int a = p[1].intVal;
	int b = p[0].intVal;

	r->intVal = a + b;
}

void printHello(UmkaStackSlot *p, UmkaStackSlot *r) {
	printf("Hello from tophat extension in c.\n");
}
