
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

void th_tmap_draw(th_tmap *t, th_rect *cam) {
	int camx = cam->x - (cam->w / 2);
	int camy = cam->y - (cam->h / 2);

	if (camx > t->x + t->w*t->cellsize) 
		return;
	if (camy > t->y + t->h*t->cellsize)
		return;

	int sx = abs((abs(t->x)-abs(camx))/t->cellsize);
	int sy = abs((abs(t->y)-abs(camy))/t->cellsize);
	int sw = cam->w/t->cellsize + 2;
	int sh = cam->h/t->cellsize + 2;

	if (t->x>=camx)
		sx = 0;
	if (t->y>=camy)
		sy = 0;

	if (sw > t->w)
		sw = t->w - sx;
	if (sh > t->h)
		sh = t->h - sy;

	for (int i=sx; i < sx+sw; i++) for (int j=sy; j < sy+sh; j++) {
		if (t->cells[j*t->w+i] == 0) continue;

		int scalex = 0;
		int scaley = 0;
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

		th_blit_tex(t->tiles[t->cells[j*t->w+i]-1]->tex, (t->x+i*t->cellsize-camx)*scaling, (t->y+j*t->cellsize-camy)*scaling, scalex*scaling+scaling/6, scaley*scaling+scaling/6, 0);
	}
}
