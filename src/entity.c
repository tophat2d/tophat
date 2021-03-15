#include <stdio.h>
#include <stdlib.h>

#include "../lib/rawdraw/CNFG.h"

#include "img/bmp.h"

#include "entity.h"
#include "rect.h"
#include "poly.h"

extern float scaling;

entity entityfrompoly(poly *p, uint32_t color) {
	entity e;
	e.p = p;
	e.color = color;
	e.img = NULL;
	e.id = rand()%9999 + 1;

	return e;
}

/*entity entityfromimage(char path[]) {
	img_t img;

	parsebmp(&path[0], &img);

	entity e;
	//e.r = newrect(0, 0, img.w, img.h);
	e.img = img.content;
	e.id = rand()%9999;

	return e;
}*/

void draw(entity *o, rect *camera) {
	int camx, camy;
	camx = camera->x - (camera->w / 2);
	camy = camera->y - (camera->h / 2);

	// it returns only if rect doesn't collide with camera.
	if (o->p->x + o->p->w < camx || o->p->x > camx + camera->w) {
		return;
	}

	if (o->p->y + o->p->h < camy || o->p->y > camy + camera->h) {
		return;
	}

	if (o->img == NULL) {
		RDPoint *points;

		points = polytordpoint(o->p, camx, camy, o->sx, o->sy, o->rot);
		CNFGColor(o->color);
		CNFGTackPoly(points, o->p->vc);
		free(points);

		return;
	}

	CNFGBlitTex(o->img->tex, o->p->x * scaling, o->p->y * scaling, o->img->w * o->sx * scaling, o->img->h * o->sy * scaling, o->rot);
	//CNFGBlitImage(o->img->rdimg, o->p->x, o->p->y, o->img->w, o->img->h);
}
