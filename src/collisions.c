// line to line is stolen from: http://jeffreythompson.org/collision-detection/table_of_contents.php
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
	uu coll = 0;
	for (uu i=0; i < 4; i++) {
		th_vf2 v = q->v[i];
		th_vf2 n = q->v[(i + 1) % 4];
		if (((v.y >= p.y && n.y < p.y) || (v.y < p.y && n.y >= p.y)) && 
			p.x < (n.x-v.x)*(p.y - v.y) / (n.y - v.y) + v.x)
			coll = !coll;
	}

	if (coll)
		*ic = p;

	return coll;
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
	th_quad q1, q2;
 	th_transform_rect(&q1, e1->t, e1->rect);
 	th_transform_rect(&q2, e2->t, e2->rect);
	
	return th_quad_to_quad(&q1, &q2, ic);
}

uu th_point_to_matrix(th_vf2 p, void/*^[]iu*/ *_m, uu w, void/*^[]bool*/ *_c,
	th_transform *t, th_vf2 *ic
) {
	UmkaDynArray(iu) *m = _m;
	UmkaDynArray(bool) *c = _c;

	// We need to remove the trasform from the point. This way we get coordinates
	// into the matrix.
	p.x -= t->pos.x;
	p.y -= t->pos.y;

	p.x /= t->scale.x;
	p.y /= t->scale.y;

	th_rotate_point(&p, t->origin, -t->rot);

	if (p.x < 0 || p.y < 0 || p.x >= w || p.y >= umkaGetDynArrayLen(m) / w)
		return 0;

	const iu tile = m->data[(uu)p.x + (uu)p.y * w];
	if (tile < 0) return 0;
	if (umkaGetDynArrayLen(c) <= tile)
		return 0;

	if (c->data[tile])
		*ic = p;
	return c->data[tile];
}
