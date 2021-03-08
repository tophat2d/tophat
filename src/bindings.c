#include <stdio.h>
#include <stdlib.h>

#include "tophat.h"
#include "../lib/rawdraw/CNFG.h"
#include "../lib/umka/src/umka_api.h"

#include "bindings.h"
#include "poly.h"

extern float scaling;
extern int *pressed;
extern int *justpressed;

void bind(void *umka) {
	// etc
	umkaAddFunc(umka, "debug", &umdebug);
	
	// input
	umkaAddFunc(umka, "cispressed", &umispressed);
	umkaAddFunc(umka, "cisjustpressed", &umisjustpressed);

	// entities
	umkaAddFunc(umka, "centdraw", &umentdraw);
	umkaAddFunc(umka, "cgetcoll", &umgetcoll);

	// misc
	umkaAddFunc(umka, "visualizecam", &umvisualizecam);

	// rawdraw
	umkaAddFunc(umka, "drawtext", &umdrawtext);
	umkaAddFunc(umka, "setup", &umCNFGSetup);
	umkaAddFunc(umka, "setbgcolor", &umCNFGSetBgColor);
	umkaAddFunc(umka, "setcolor", &umCNFGSetColor);
	umkaAddFunc(umka, "clearframe", &umCNFGClearFrame);
	umkaAddFunc(umka, "getdimensions", &umCNFGGetDimensions);
	umkaAddFunc(umka, "swapbuffers", &umCNFGSwapBuffers);
	umkaAddFunc(umka, "handleinput", &umCNFGHandleInput);
	umkaAddFunc(umka, "updatescaling", &umgetscaling);
}

// etc
void umdebug(UmkaStackSlot *p, UmkaStackSlot *r) {
	// prints polygon
	/*printf("polyx: %d, polyy: %d, \n", e->p->x, e->p->y);

	for (int i=0; i < e->p->vc * 2; i += 2) {
		printf("x: %d, y: %d\n", e->p->v[i], e->p->v[i + 1]);
	}*/
}

// input
void umispressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	//printf("%d\n", pressed[keycode]);

	r[0].intVal = pressed[keycode];
}

void umisjustpressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r[0].intVal = justpressed[keycode];
}

// entities
void umentdraw(UmkaStackSlot *p, UmkaStackSlot *r) {
	rect *rc = (rect *)&p[0];
	entity *e = rc + sizeof(UmkaStackSlot *)/sizeof(UmkaStackSlot); // this is weird solution, but it seems to work for now. TODO

	draw(e, rc);
}

void umgetcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	entity *scene = (entity *)p[0].ptrVal;
	entity *e = (entity *)p[1].ptrVal;
	int count = p[2].intVal;
	int coll;

	//printf("%d, %X, %d\n", e->p->x, e->color, sizeof(poly *));

	for (int i=0; i < count; i++) {
		if (e->id == scene[i].id) {
			continue;
		}

		coll = polytopoly(scene[i].p, e->p);
		if (coll) {
			r->intVal = scene[i].id;
			return;
		}
	}
	r->intVal = 0;
}

// misc
void umvisualizecam(UmkaStackSlot *p, UmkaStackSlot *r) {
	int w = p[2].intVal;
	int h = p[1].intVal;
	int color = p[0].uintVal;

	CNFGColor((uint32_t)color);
	CNFGTackRectangle(0, 0, w * scaling, h * scaling);
}

// rawdraw
void umdrawtext(UmkaStackSlot *p, UmkaStackSlot *r) {
	float size = p[0].realVal;
	uint32_t color = (uint32_t)p[1].uintVal;
	int y = (int)p[2].intVal;
	int x = (int)p[3].intVal;
	char *text = (char *)p[4].ptrVal;

	CNFGPenX = x;
	CNFGPenY = y;
	CNFGColor(color);
	CNFGDrawText(text, size * scaling);
}

void umCNFGSetup(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *title = (char *)p[2].ptrVal;
	int w = p[1].intVal;
	int h = p[0].intVal;

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
	int *w = p[1].ptrVal;
	int *h = p[0].ptrVal;
	short ws, hs;

	CNFGGetDimensions(&ws, &hs);

	*w = ws;
	*h = hs;
}

void umCNFGSwapBuffers(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGSwapBuffers();
}

void umCNFGHandleInput(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGHandleInput();
}

void umgetscaling(UmkaStackSlot *p, UmkaStackSlot *r) {
	int camh = p[0].intVal;
	int camw = p[1].intVal;
	int h = p[2].intVal;
	int w = p[3].intVal;

	if ((float)w/camw < (float)h/camh) {
		scaling = ((float)w/camw);
	} else {
		scaling = ((float)h/camh);
	}
}

