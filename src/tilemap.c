
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <CNFG.h>

#include "tophat.h"

extern float scaling;

enum {
	STRETCH,
	TOPLEFT,
} scaleops;

void th_tmap_draw(th_tmap *t, th_rect *cam) {
	int camx = cam->x - (cam->w / 2);
	int camy = cam->y - (cam->h / 2);

	th_transform tr = {0};

	if (camx > t->pos.x + t->w*t->cellsize)
		return;
	if (camy > t->pos.y + t->h*t->cellsize)
		return;

	int sx = fabs((fabs(t->pos.x)-abs(camx)))/t->cellsize;
	int sy = fabs((fabs(t->pos.y)-abs(camy)))/t->cellsize;
	int sw = cam->w/t->cellsize + 2;
	int sh = cam->h/t->cellsize + 2;

	if (t->pos.x>=camx)
		sx = 0;
	if (t->pos.y>=camy)
		sy = 0;

	if (sw > t->w)
		sw = t->w - sx;
	if (sh > t->h)
		sh = t->h - sy;

	for (int i=sx; i < sx+sw; i++) for (int j=sy; j < sy+sh; j++) {
		if (t->cells[j*t->w+i] == 0) continue;

		switch (t->scaletype) {
		case STRETCH:
			tr.scale.x = t->cellsize * scaling;
			tr.scale.y = t->cellsize * scaling;
			break;
		case TOPLEFT:
			tr.scale.x = t->tiles[t->cells[j*t->w+i]-1]->dm.w * scaling;
			tr.scale.y = t->tiles[t->cells[j*t->w+i]-1]->dm.h * scaling;
			break;
		}

		tr.pos.x = (t->pos.x+i*t->cellsize-camx)*scaling;
		tr.pos.y = (t->pos.y+j*t->cellsize-camy)*scaling;
		const th_quad q = th_ent_transform(
			&(th_ent){ .rect = (th_rect){ .w = 1, .h = 1}, .t = tr});
		th_blit_tex(t->tiles[t->cells[j*t->w+i]-1]->gltexture, q);
	}
}
