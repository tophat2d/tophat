#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "tophat.h"
#include "../lib/rawdraw/CNFG.h"
#include "../lib/umka/src/umka_api.h"
#include "../lib/stb/stb_image.h"

#include "bindings.h"
#include "poly.h"

extern float scaling;
extern int *pressed;
extern int *justpressed;
extern int mx;
extern int my;

extern char *respath;

void umkabind(void *umka) {
	// etc
	umkaAddFunc(umka, "debug", &umdebug);
	umkaAddFunc(umka, "debug2", &umdebug2);

	// images
	umkaAddFunc(umka, "loadimg", &umimgload);
	umkaAddFunc(umka, "deleteimg", &umimgfree);
	//umkaAddFunc(umka, "imgsetscale", &umimgsetscale);
	//umkaAddFunc(umka, "imgrotate", &umimgrotate);

	// input
	umkaAddFunc(umka, "cgetmouse", &umgetmouse);
	umkaAddFunc(umka, "cispressed", &umispressed);
	umkaAddFunc(umka, "cisjustpressed", &umisjustpressed);

	// entities
	umkaAddFunc(umka, "centdraw", &umentdraw);
	umkaAddFunc(umka, "cgetcoll", &umgetcoll);

	// misc
	umkaAddFunc(umka, "sleep", &umsleep);
	umkaAddFunc(umka, "visualizecam", &umvisualizecam);
	umkaAddFunc(umka, "gettime", &umgettime);

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
}

void umdebug2(UmkaStackSlot *p, UmkaStackSlot *r) {
}

// images
void umimgload(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *path = (char *)p[0].ptrVal;

	image *img = loadimage(strcat(respath, path));
	rdimg(img, scaling);
	img->tex = CNFGTexImage(img->rdimg, img->w, img->h);

	r[0].ptrVal = (long int)img;
}
void umimgfree(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[0].ptrVal;

	free(img->rdimg);
	stbi_image_free(img);
}
/*void umimgsetscale(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[0].ptrVal;
	double x = p[1].realVal;
	double y = p[2].realVal;

	img->scalex = x;
	img->scaley = y;
}
void umimgrotate(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[0].ptrVal;
	int rot = p[1].intVal;

	img->rot = rot;
}*/

// input
void umgetmouse(UmkaStackSlot *p, UmkaStackSlot *r) {
	int *x = (int *)p[1].ptrVal;
	int *y = (int *)p[0].ptrVal;

	*x = mx / scaling;
	*y = my / scaling;
}

void umispressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	//printf("%d\n", pressed[keycode]);

	r[0].intVal = pressed[keycode];
}

void umisjustpressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r[0].intVal = justpressed[keycode];
	justpressed[keycode] = 0;
}

// entities
void umentdraw(UmkaStackSlot *p, UmkaStackSlot *r) {
	rect *rc = (rect *)&p[0];
	entity *e = (entity *)(rc + sizeof(UmkaStackSlot *)/sizeof(UmkaStackSlot)); // this is weird solution, but it seems to work for now. TODO

	if (e->img == 0) {
		e->img = NULL;
	}

	draw(e, rc);
}

void umgetcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	entity **scene = (entity **)p[0].ptrVal;
	entity *e = (entity *)p[1].ptrVal;
	int count = p[2].intVal;
	int coll;

	//printf("%d, %X, %d\n", e->p->x, e->color, sizeof(poly *));

	int px, py, sx, sy;

	for (int i=0; i < count; i++) {
		if (e->id == scene[i]->id) {
			continue;
		}

		px = e->p->x;
		py = e->p->y;
		rotatepoint(&px, &py, e->p->w/2, e->p->h/2, e->rot);
		sx = e->p->x;
		sy = e->p->y;
		rotatepoint(&sx, &sy, scene[i]->p->w/2, scene[i]->p->h/2, scene[i]->rot);

		if (px > (sx * e->sx + scene[i]->p->w) * scene[i]->sx) {
			continue;
		}

		if (py > sy * (e->sy + scene[i]->p->h) * scene[i]->sy) {
			continue;
		}

		if ((e->p->w + px) * e->sx < sx * scene[i]->sx) {
			continue;
		}

		if ((e->p->h + py) * e->sx < sy * scene[i]->sy) {
			continue;
		}

		coll = polytopoly(scene[i]->p, e->p, scene[i]->rot, e->rot, scene[i]->sx, scene[i]->sy, e->sx, e->sy);
		if (coll) {
			r->intVal = scene[i]->id;
			return;
		}

		coll = polytopoly(e->p, scene[i]->p, scene[i]->rot, e->rot, scene[i]->sx, scene[i]->sy, e->sx, e->sy);
		if (coll) {
			r->intVal = scene[i]->id;
			return;
		}
	}
	r->intVal = 0;
}

// misc

void umsleep(UmkaStackSlot *p, UmkaStackSlot *r) {
	int t = p[0].intVal;

	slp(t);
}

void umvisualizecam(UmkaStackSlot *p, UmkaStackSlot *r) {
	int w = p[2].intVal;
	int h = p[1].intVal;
	int color = p[0].uintVal;

	CNFGColor((uint32_t)color);
	CNFGTackRectangle(0, 0, w * scaling, h * scaling);
}

void umgettime(UmkaStackSlot *p, UmkaStackSlot *r) {
	struct timeval t;
	gettimeofday(&t, NULL);

	r->intVal = (long int)(t.tv_usec);
}

// rawdraw
void umdrawtext(UmkaStackSlot *p, UmkaStackSlot *r) {
	float size = p[0].realVal;
	uint32_t color = (uint32_t)p[1].uintVal;
	int y = (int)p[2].intVal;
	int x = (int)p[3].intVal;
	char *text = (char *)p[4].ptrVal;

	CNFGPenX = x * scaling;
	CNFGPenY = y * scaling;
	CNFGColor(color);
	CNFGSetLineWidth(0.6 * scaling);
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
	int *w = (int *)p[1].ptrVal;
	int *h = (int *)p[0].ptrVal;
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

