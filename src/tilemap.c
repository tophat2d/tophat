#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "../lib/rawdraw/CNFG.h"

#include "tilemap.h"
#include "rect.h"

extern float scaling;

enum {
	STRETCH,
	TOPLEFT,
} scaleops;

void tmapdraw(tmap *t, rect *cam) {
	int camx = cam->x - (cam->w / 2);
	int camy = cam->y - (cam->h / 2);

	if (camx > t->w*t->cellsize) return;
	if (camy > t->h*t->cellsize) return;

	int sx = abs((abs(t->x)-abs(camx))/t->cellsize - 1);
	int sy = abs((abs(t->y)-abs(camy))/t->cellsize - 1);
	int sw = cam->w/t->cellsize + 1;
	int sh = cam->h/t->cellsize + 1;

	if (t->x>=camx) sx = 0;
	if (t->y>=camy) sy = 0;

	if (sw > t->w) sw = t->w;
	if (sh > t->h) sh = t->h;

	for (int i=sx; i < sx+sw; i++) for (int j=sy; j < sy+sh; j++) {
		if (t->cells[j*t->w+i] == 0) {
			continue;
		}
		int scalex, scaley;
		switch (t->scaletype) {
		case STRETCH:
			scalex = t->cellsize;
			scaley = t->cellsize;
			break;
		case TOPLEFT:
			scalex = t->tiles[t->cells[j*t->w+i]-1]->w;
			scaley = t->tiles[t->cells[j*t->w+i]-1]->h;
			break;
		}

		//CNFGBlitTex(t->tiles[t->cells[j*t->w+i]-1]->tex, (t->x+i*t->cellsize-camx)*scaling, (t->y+j*t->cellsize-camy)*scaling, scalex*scaling+scaling/6, scaley*scaling+scaling/6, 0);
		blittex(t->tiles[t->cells[j*t->w+i]-1]->tex, (t->x+i*t->cellsize-camx)*scaling, (t->y+j*t->cellsize-camy)*scaling, scalex*scaling+scaling/6, scaley*scaling+scaling/6, 0);
	}
}
