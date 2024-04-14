#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "tophat.h"

#define MIN(a, b) (a < b ? a : b)

// line to line is stolen from: http://jeffreythompson.org/collision-detection/table_of_contents.php
int
th_line_to_line(th_vf2 b1, th_vf2 e1, th_vf2 b2, th_vf2 e2, th_vf2 *ic1)
{
	const float x1 = b1.x, y1 = b1.y, x2 = e1.x, y2 = e1.y, x3 = b2.x, y3 = b2.y, x4 = e2.x,
		    y4 = e2.y;
	float uA = (float)((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) /
	    ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
	float uB = (float)((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) /
	    ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
		ic1->x = b1.x + (uA * (e1.x - b1.x));
		ic1->y = b1.y + (uA * (e1.y - b1.y));
		return 1;
	}

	return 0;
}

uu
th_point_to_quad(th_vf2 p, th_quad *q, th_vf2 *ic)
{
	uu coll = 0;
	for (uu i = 0; i < 4; i++) {
		th_vf2 c = q->v[i];
		th_vf2 n = q->v[(i + 1) % 4];
		if (((c.y >= p.y && n.y < p.y) || (c.y < p.y && n.y >= p.y)) &&
		    (p.x < (n.x - c.x) * (p.y - c.y) / (n.y - c.y) + c.x))
			coll = !coll;
	}

	*ic = p;
	return coll;
}

static fu
point_distance(th_vf2 a, th_vf2 b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a.x * a.x + a.y * a.y;
}

uu
th_line_to_quad(th_vf2 b, th_vf2 e, th_quad *q, th_vf2 *ic1, th_vf2 *ic2)
{
	for (uu i = 0; i < 4; i++) {
		if (th_line_to_line(b, e, q->v[i], q->v[(i + 1) % 4], ic1)) {
			*ic2 = *ic1;

			for (i++; i < 4; i++)
				if (th_line_to_line(b, e, q->v[i], q->v[(i + 1) % 4], ic2))
					break;
			return true;
		}
	}

	if (th_point_to_quad(b, q, ic1) || th_point_to_quad(e, q, ic2)) {
		*ic1 = b;
		*ic2 = e;
		return true;
	}

	return false;
}

uu
th_rect_to_rect(th_rect *r1, th_rect *r2)
{
	return r1->x + r1->w >= r2->x && r1->x <= r2->x + r2->w && r1->y + r1->h >= r2->y &&
	    r1->y <= r2->y + r2->h;
}

uu
th_quad_to_quad(th_quad *q1, th_quad *q2, th_vf2 *ic)
{
	th_rect r1 = th_quad_bounding_box(*q1);
	th_rect r2 = th_quad_bounding_box(*q2);
	if (!th_rect_to_rect(&r1, &r2))
		return 0;

	for (uu i = 0; i < 4; i++)
		if (th_line_to_quad(q1->v[i], q1->v[(i + 1) % 4], q2, ic, ic))
			return 1;

	for (uu i = 0; i < 4; i++)
		if (th_point_to_quad(q1->v[i], q2, ic))
			return 1;

	for (uu i = 0; i < 4; i++)
		if (th_point_to_quad(q2->v[i], q1, ic))
			return 1;

	return 0;
}

uu
th_rect_to_quad(th_rect *r, th_quad *q, th_vf2 *ic)
{
	th_quad q2 = {
	    .tl = {.x = r->x, .y = r->y},
	    .tr = {.x = r->x + r->w, .y = r->y},
	    .br = {.x = r->x + r->w, .y = r->y + r->h},
	    .bl = {.x = r->x, .y = r->y + r->h},
	};
	return th_quad_to_quad(&q2, q, ic);
}

uu
th_ent_to_ent(th_ent *e1, th_ent *e2, th_vf2 *ic)
{
	th_quad q1, q2;
	th_transform_rect(&q1, e1->t, e1->rect);
	th_transform_rect(&q2, e2->t, e2->rect);

	return th_quad_to_quad(&q1, &q2, ic);
}

uu
th_line_to_tilemap(th_vf2 b, th_vf2 e, th_tmap *t, th_vf2 *ic)
{
	int tw = t->w, th = umkaGetDynArrayLen(&t->cells) / t->w;

	float mx = e.x - b.x, my = e.y - b.y;
	float lineLen = sqrt(mx * mx + my * my);
	th_vector_normalize(&mx, &my);

	float x = b.x, y = b.y, minlen = -1;
	bool coll = false;
	float len = 0;
	while (len < lineLen) {
		int tx = (x - t->pos.x) / (t->scale * t->a.cs.x);
		int ty = (y - t->pos.y) / (t->scale * t->a.cs.y);

		int tile = t->cells.data[(int)(ty * t->w + tx)] - 1;
		if (x >= t->pos.x && y >= t->pos.y && tx < tw && ty < th && tile >= 0 &&
		    t->collmask.data[tile]) {
			coll = true;
			if (minlen == -1 || len < minlen)
				minlen = len;
		}

		x += mx;
		y += my;
		len += sqrt(mx * mx + my * my);
	}

	ic->x = b.x + (e.x - b.x) / lineLen * minlen;
	ic->y = b.y + (e.y - b.y) / lineLen * minlen;

	return coll;
}

// This can fail if entity is bigger than a tile. TODO
uu
th_coll_on_tilemap(th_ent *e, th_tmap *t, th_vf2 *ic, th_vf2 *tc)
{
	th_quad q = th_ent_transform(e);
	th_rect bb = th_quad_bounding_box(q);

	bb.w = (bb.x + bb.w - t->pos.x) / (t->scale * t->a.cs.x);
	bb.h = (bb.y + bb.h - t->pos.y) / (t->scale * t->a.cs.y);
	bb.x = trunc((bb.x - t->pos.x) / (t->scale * t->a.cs.x));
	bb.y = trunc((bb.y - t->pos.y) / (t->scale * t->a.cs.y));
	bb.w = ceil(bb.w - bb.x);
	bb.h = ceil(bb.h - bb.y);

	const int tw = t->w;
	const int th = umkaGetDynArrayLen(&t->cells) / t->w;

	if (bb.x < 0)
		bb.x = 0;
	if (bb.y < 0)
		bb.y = 0;
	if (bb.x >= tw)
		return 0;
	if (bb.y >= th)
		return 0;

	for (int x = bb.x; x < tw && x < bb.x + bb.w; x++) {
		for (int y = bb.y; y < th && y < bb.y + bb.h; y++) {
			const int tile = t->cells.data[(tw * y) + x];
			if (!tile)
				continue;
			if (!t->collmask.data[tile - 1])
				continue;

			th_rect r = {
			    .x = x * t->scale * t->a.cs.x + t->pos.x,
			    .y = y * t->scale * t->a.cs.y + t->pos.y,
			    .w = t->scale * t->a.cs.x,
			    .h = t->scale * t->a.cs.y,
			};
			if (th_rect_to_quad(&r, &q, ic)) {
				*tc = (th_vf2){.x = x, .y = y};
				return 1;
			}
		}
	}

	return 0;
}

uu
th_coll_point_on_rect(th_vf2 p, th_rect *r)
{
	return p.x >= r->x && p.y >= r->y && p.x <= r->x + r->w && p.y <= r->y + r->h;
}
