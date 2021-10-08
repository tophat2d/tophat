//
// largely inspired by (copied from :]) this guide: http://jeffreythompson.org/collision-detection/table_of_contents.php
//
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "tophat.h"

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

int th_line_to_line(th_vf2 b1, th_vf2 e1, th_vf2 b2, th_vf2 e2, th_vf2 *ic) {
	const float
		x1 = b1.x, y1 = b1.y,
		x2 = e1.x, y2 = e1.y,
		x3 = b2.x, y3 = b2.y,
		x4 = e2.x, y4 = e2.y;
	float uA = (float)((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
	float uB = (float)((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));

	ic->x = b1.x + (uA * (e1.x-b1.x));
	ic->y = b1.y + (uA * (e1.y-b1.y));

	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
		return 1;

	return 0;
}

uu th_point_to_quad(th_vf2 p, th_quad *q, th_vf2 *ic) {
	// TODO
	return 0;
}

uu th_line_to_quad(th_vf2 b, th_vf2 e, th_quad *q, th_vf2 *ic) {
	for (uu i=0; i < 4; i++)
		if (th_line_to_line(b, e, q->v[i], q->v[(i+1)%4], ic))
			return 1;

	return 0;
}

uu th_quad_to_quad(th_quad *q1, th_quad *q2, th_vf2 *ic) {
	for (uu i=0; i < 4; i++)
		if (th_line_to_quad(q1->v[i], q1->v[(i+1)%4], q2, ic))
			return 1;

	for (uu i=0; i < 4; i++)
		if (th_point_to_quad(q1->v[i], q2, ic))
			return 1;

	return 0;
}

uu th_ent_to_ent(th_ent *e1, th_ent *e2, th_vf2 *ic) {
	th_quad q1 = th_ent_transform(e1);
	th_quad q2 = th_ent_transform(e2);
	
	return th_quad_to_quad(&q1, &q2, ic);
}

bool th_ray_to_tilemap(th_ray *ra, th_tmap *t, th_vf2 *ic) {
	th_vf2
		b = ra->pos,
		e = ra->pos;
	e.y += ra->l;

	th_rotate_point(&e, ra->pos, ra->r);

	float
		mx = e.x - b.x,
		my = e.y - b.y;
	th_vector_normalize(&mx, &my);

	float
		x = b.x,
		y = b.y,
		minlen = -1;
	bool coll = false;
	float len = 0;
	while (len < ra->l) {
		int tx = (x - t->pos.x) / t->cellsize;
		int ty = (y - t->pos.y) / t->cellsize;

		int tile = t->cells[(int)(ty * t->w + tx)] - 1;
		if (x >= t->pos.x && y >= t->pos.y && tx < t->w && ty < t->h &&
			tile >= 0 && t->collmask[tile]) {
			coll = true;
			if (minlen == -1 || len < minlen)
				minlen = len;
		}

		x += mx;
		y += my;
		len += sqrt(mx * mx + my * my);
	}	

	*ic = ra->pos;
	ic->y += minlen;
	th_rotate_point(ic, ra->pos, ra->r);

	return coll;
}

// This can fail if entity is bigger than a tile. TODO
uu _th_coll_on_tilemap(th_ent *e, th_tmap *t, uu *vert, th_vf2 *tc) {
	th_quad q = th_ent_transform(e);

	for (uu i=0; i < 4; i++) {
		const iu tx = (q.v[i].x - t->pos.x) / t->cellsize;
		const iu ty = (q.v[i].y - t->pos.y) / t->cellsize;

		if (q.v[i].x < t->pos.x || q.v[i].y < t->pos.x) continue;
		if (tx < 0 || ty < 0) continue;
		if (tx >= t->w || ty >= t->h) continue;

		const int tile = t->cells[(t->w * ty) + tx];
		if (!tile)
			continue;

		if (t->collmask[tile - 1]) {
			*vert = i;
			tc->x = tx;
			tc->y = ty;
			return 1;
		}
	}

	return 0;
}

