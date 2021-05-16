#include <stdio.h>
#include <stdlib.h>

#include "../lib/rawdraw/CNFG.h"

#include "entity.h"
#include "rect.h"
#include "poly.h"
#include "misc.h"

extern float scaling;

void draw(entity *o, rect *camera) {
	int camx, camy;
	camx = camera->x - (camera->w / 2);
	camy = camera->y - (camera->h / 2);

	if (o->p->x + o->p->w < camx || o->p->x - o->p->w > camx + camera->w)
		return;

	if (o->p->y + o->p->h < camy || o->p->y - o->p->h > camy + camera->h)
		return;

	if (o->img == NULL) {
		if (o->p == NULL) {
			char buff[128];
			sprintf(buff, "invalid entity with id of %d", o->id);
			errprint(buff);
		}

		RDPoint *points;

		points = polytordpoint(o->p, camx, camy);
		CNFGColor(o->color);
		CNFGTackPoly(points, o->p->vc);
		free(points);

		return;
	}

	blittex(o->img->tex, (o->p->x-camx) * scaling, (o->p->y-camy) * scaling, o->img->w * o->sx * scaling, o->img->h * o->sy * scaling, o->rot);
	//CNFGBlitTex(o->img->tex, (o->p->x-camx) * scaling, (o->p->y-camy) * scaling, o->img->w * o->sx * scaling, o->img->h * o->sy * scaling, o->rot);
}
