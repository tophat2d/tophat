#include <stdio.h>
#include <stdlib.h>
#include <CNFG.h>

#include "tophat.h"

extern float scaling;

void th_ent_draw(th_ent *o, th_rect *camera) {
	int camx, camy;
	camx = camera->x - (camera->w / 2);
	camy = camera->y - (camera->h / 2);

	if (o->p.x + o->p.w < camx || o->p.x - o->p.w > camx + camera->w)
		return;

	if (o->p.y + o->p.h < camy || o->p.y - o->p.h > camy + camera->h)
		return;

	if (o->img == NULL) {
		RDPoint *points;

		points = _th_poly_to_rdpoint(&o->p, camx, camy);
		CNFGColor(o->color);
		CNFGTackPoly(points, o->p.vc);
		free(points);

		return;
	}

	th_blit_tex(o->img->tex,
	           (o->p.x-camx) * scaling,
	           (o->p.y-camy) * scaling,
						  o->img->w * o->sx * scaling,
							o->img->h * o->sy * scaling,
							o->rot);
}

int th_ent_getcoll(th_ent *e, th_ent *scene, int count, int *ix, int *iy) {
	int coll;

	int ex = e->p.x;
	int ey = e->p.y;
	int ew = e->p.w;
	int eh = e->p.h;

	if (!ex || !ey)
		return 0;

	if (ew < 0)
		ew = abs(ew);
	if (eh < 0)
		eh = abs(eh);

	for (int i=0; i < count; i++) {
		if (e->id == scene[i].id) continue;

		int sx = e->p.x;
		int sy = e->p.y;
		int sw = e->p.w;
		int sh = e->p.h;

		if (sw < 0)
			sw = abs(sw);
		if (sh < 0)
			sh = abs(sh);

		if (ex > sx + sw) continue;
		if (ey > sy + sh) continue;
		if (ew + ex < sx) continue;
		if (eh + ey < sy) continue;

		coll = _th_poly_to_poly(&scene[i].p, &e->p, ix, iy);
		if (coll) {
			return scene[i].id;
		}

		coll = _th_poly_to_poly(&e->p, &scene[i].p, ix, iy);
		if (coll) {
			return scene[i].id;
		}
	}
	return 0;
}
