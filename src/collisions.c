//
// largely inspired by (copied from :]) this guide: http://jeffreythompson.org/collision-detection/table_of_contents.php
//
#include "entity.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "collisions.h"
#include "polygon.h"
#include "tilemap.h"

int stepify(int inp, int step);

// checks collision between two polygons
int _th_poly_to_poly(th_poly *p1, th_poly *p2, int *ix, int *iy) {
	int next = 0;
	int vcx, vcy, vnx, vny;
	int coll = 0;

	for (int current = 0; current < p1->vc * 2; current += 2) {
		next = current + 2;

		if (next >= p1->vc)
			next = 0;

		vcx = p1->v[current] + p1->x;
		vcy = p1->v[current + 1] + p1->y;
		vnx = p1->v[next] + p1->x;
		vny = p1->v[next + 1] + p1->y;

		coll = _th_poly_to_line(p2, vcx, vcy, vnx, vny, ix, iy);
		if (coll)
			return 1;

		coll = _th_poly_to_point(p1, p2->x + p2->v[0], p2->y + p2->v[1], ix, iy);
		if (coll)
			return 1;
	}

	return 0;
}

int _th_poly_to_line(th_poly *a, int sx, int sy, int ex, int ey, int *ix, int *iy) {
	int next = 0;
	int csx, csy, cex, cey;
	int coll = 0;

	for (int current = 0; current < a->vc*2; current += 2) {
		next = current + 2;

		csx = a->v[current] + a->x;
		csy = a->v[current + 1] + a->y;
		cex = a->v[next] + a->x;
		cey = a->v[next + 1] + a->y;

		coll = _th_line_to_line(sx, sy, ex, ey, csx, csy, cex, cey, ix, iy);
		if (coll)
			return 1;
	}

	return 0;
}

int _th_line_to_line(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int *ix, int *iy) {
	float uA = (float)((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
	float uB = (float)((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));

	*ix = x1 + (uA * (x2-x1));
	*iy = y1 + (uA * (y2-y1));

	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
		return 1;

	return 0;
}

int _th_poly_to_point(th_poly *a, int px, int py, int *ix, int *iy) {
	int result = 0;
	int current, next;
	int vcx, vnx, vcy, vny;

	next = 0;

	for (current = 0; current <= a->vc*2; current += 2) {
		next = current + 2;

		if (next == a->vc*2)
			next = 0;

		vcx = a->v[current] + a->x;
		vnx = a->v[next] + a->x;
		vcy = a->v[current+1] + a->y;
		vny = a->v[next+1] + a->y;

		if (((vcy >= py && vny < py) || (vcy < py && vny >= py)) && (px < (vnx-vcx)*(py-vcy) / (vny-vcy)+vcx)) {
			*ix = vcx;
			*iy = vcy;
			result = !result;
		}
	}
	return result;
}

int stepify(int inp, int step) {
	if (step != 0)
		inp = trunc(inp / step + 0.5) * step;
	return inp;
}

bool _th_coll_on_tilemap(th_poly *p, th_tmap *t, int *rx, int *ry) {
	if (p->x < t->x || p->y < t->y)
		return false;

	for (int i=0; i < p->vc * 2; i += 2) {
		int absx = p->x + p->v[i];
		int absy = p->y + p->v[i+1];
		int tx = (absx - t->x) / t->cellsize;
		int ty = (absy - t->y) / t->cellsize;

		int tile = t->cells[(t->w * ty) + tx];
		//printf("abs: %d %d\nt: %d %d\ntilen: %d\ncoll: %d\n---------\n", absx, absy, tx, ty, tile, t->collmask[tile]);
		if (t->collmask[tile - 1]) {
			*rx = absx;
			*ry = absy;
			return true;
		}
	}
	return false;
}

