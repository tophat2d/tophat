#include <stdio.h>
#include <stdlib.h>

#include "tophat.h"
#include "../lib/rawdraw/CNFG.h"
#include "../lib/umka/src/umka_api.h"

#include "bindings.h"
#include "poly.h"

void bind(void *umka) {
	umkaAddFunc(umka, "debug", &umdebug);

	umkaAddFunc(umka, "centdraw", &umentdraw);

	// rawdraw
	umkaAddFunc(umka, "setup", &umCNFGSetup);
	umkaAddFunc(umka, "setbgcolor", &umCNFGSetBgColor);
	umkaAddFunc(umka, "setcolor", &umCNFGSetColor);
	umkaAddFunc(umka, "clearframe", &umCNFGClearFrame);
	umkaAddFunc(umka, "getdimensions", &umCNFGGetDimensions);
	umkaAddFunc(umka, "swapbuffers", &umCNFGSwapBuffers);
	umkaAddFunc(umka, "handleinput", &umCNFGHandleInput);
}

void umdebug(UmkaStackSlot *p, UmkaStackSlot *r) {
	
	entity *e = (entity *)&p[0];


	//poly *pl = (poly *)&p[0];

	//printf("x: %d, y: %d, vc: %d, v1x: %d, v1y: %d\n", pl->x, pl->y, pl->vc, &pl->v[0], &pl->v[1]);
	//printf("comparison: %d, %d\n", pl->v, 0);//p[1].intVal);

	printf("polyx: %d, polyy: %d, \n", e->p->x, e->p->y);

	for (int i=0; i < e->p->vc * 2; i += 2) {
		printf("x: %d, y: %d\n", e->p->v[i], e->p->v[i + 1]);
	}

	rect rc = newrect(20, 20, 100, 100);

	draw(e, &rc);
}

void umentdraw(UmkaStackSlot *p, UmkaStackSlot *r) {
	entity *e = (entity *)&p[1];
	rect *rc = (rect *)&p[0];

	//printf("polyx: %d, polyy: %d, \n", e->p->x, e->p->y);

	/*for (int i=0; i < e->p->vc * 2; i += 2) {
		printf("x: %d, y: %d\n", e->p->v[i], e->p->v[i + 1]);
	}*/

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
