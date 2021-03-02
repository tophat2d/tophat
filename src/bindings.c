#include <stdio.h>
#include <stdlib.h>

#include "tophat.h"
#include "../lib/rawdraw/CNFG.h"
#include "../lib/umka/src/umka_api.h"

#include "bindings.h"
#include "poly.h"

void bind(void *umka) {
	// entities
	umkaAddFunc(umka, "entityfrompoly", &umentityfrompoly);
	umkaAddFunc(umka, "cpolydraw", &umpolydraw);

	// polygons
	umkaAddFunc(umka, "newpoly", &umnewpoly);
	umkaAddFunc(umka, "cpolysetpos", &umpolysetpos);

	// rawdraw
	umkaAddFunc(umka, "setup", &umCNFGSetup);
	umkaAddFunc(umka, "setbgcolor", &umCNFGSetBgColor);
	umkaAddFunc(umka, "setcolor", &umCNFGSetColor);
	umkaAddFunc(umka, "clearframe", &umCNFGClearFrame);
	umkaAddFunc(umka, "getdimensions", &umCNFGGetDimensions);
	umkaAddFunc(umka, "swapbuffers", &umCNFGSwapBuffers);
	umkaAddFunc(umka, "handleinput", &umCNFGHandleInput);
}

// entities

void umentityfrompoly(UmkaStackSlot *p, UmkaStackSlot *r) {
	poly *pl = (poly *)p[0].intVal;
	uint32_t color = (uint32_t)p[0].intVal;

	entity *e;
	e = malloc(sizeof(entity));

	e->p = pl;
	e->c = color;

	r[0].intVal = (intptr_t)pl;
}

void umpolydraw(UmkaStackSlot *p, UmkaStackSlot *r) {
	draw(*(entity *)p[1].intVal, *(rect *)p[0].intVal);
}

// polygons
void umnewpoly(UmkaStackSlot *p, UmkaStackSlot *r) {
	poly *pl;
	pl = malloc(sizeof(poly));
	int count = p[0].intVal * 2 + 3;
	int *v;
	v = malloc(sizeof(int) * count - 3);

	pl->x = p[count-1].intVal;
	pl->y = p[count-2].intVal;
	pl->vc = count - 3;

	for (int i=0; i < count - 3; i++) {
		v[i] = p[i + 1].intVal;
	}
	pl->v = v;

	r[0].intVal = (intptr_t)pl;
}

void umpolysetpos(UmkaStackSlot *p, UmkaStackSlot *r) {
	poly *pl = (poly *)p[0].intVal;

	pl->y = p[1].intVal;
	pl->x = p[2].intVal;
}

void umCNFGSetup(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *title = (char *)p[2].ptrVal;
	int w = p[1].intVal;
	int h = p[0].intVal;

	printf("w: %d, h: %d\n", w, h);

	int res = CNFGSetup(title, w, h);

	if (res) {
		printf("could not initialize rawdraw\n");
	}
}

void umCNFGSetBgColor(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGBGColor = (uint32_t)p[0].uintVal;
}

void umCNFGSetColor(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGColor((uint32_t)p[0].uintVal);
}

void umCNFGClearFrame(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGClearFrame();
}

void umCNFGGetDimensions(UmkaStackSlot *p, UmkaStackSlot *r) {
	short w, h;
	CNFGGetDimensions(&w, &h);
	r[0].intVal = (int)h;
	r[1].intVal = (int)w;
}

void umCNFGSwapBuffers(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGSwapBuffers();
}

void umCNFGHandleInput(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGHandleInput();
}
