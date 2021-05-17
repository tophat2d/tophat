//
// largely inspired by (copied from :]) this guide: http://jeffreythompson.org/collision-detection/table_of_contents.php
//
#include "entity.h"
#include <stdio.h>
#include <math.h>

#include "collisions.h"
#include "main.h"
#include "poly.h"
#include "tilemap.h"

int stepify(int inp, int step);

extern int entitycount;

// relic from the times tophat was c only. I'm keeping this, since I want to make tophat work with c.
int collbyentity(entnode_t *a, entity *e) {
	entnode_t *current, *next;
	int i = 0;
	int coll;

	current = a;
	next = a;

	if (e == NULL) {
		return 1;
	}

	i = 0;

	while (next != NULL) {

		current = next;
		next = current->next;

		i++;

		if (current->val == NULL) {
			current = current->next;
			continue;
		}

		if (e->id == current->val->id) {
			continue;
		}

		int ix, iy;
		coll = polytopoly(current->val->p, e->p, &ix, &iy);
		if (coll) {
			return current->val->id;
		}

		if (current->next == NULL) {
			return 0;
		}
	}
	return 0;
}

// checks collision between two polygons
int polytopoly(poly *p1, poly *p2, int *ix, int *iy) {
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

		coll = polytoline(p2, vcx, vcy, vnx, vny, ix, iy);
		if (coll) return 1;

		coll = polytopoint(p1, p2->x + p2->v[0], p2->y + p2->v[1], ix, iy);
		if (coll) return 1;
	}

	return 0;
}

int polytoline(poly *a, int sx, int sy, int ex, int ey, int *ix, int *iy) {
	int next = 0;
	int csx, csy, cex, cey;
	int coll = 0;

	for (int current = 0; current < a->vc*2; current += 2) {
		next = current + 2;

		csx = a->v[current] + a->x;
		csy = a->v[current + 1] + a->y;
		cex = a->v[next] + a->x;
		cey = a->v[next + 1] + a->y;

		coll = linetoline(sx, sy, ex, ey, csx, csy, cex, cey, ix, iy);
		if (coll)
			return 1;
	}

	return 0;
}

int linetoline(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int *ix, int *iy) {
	float uA = (float)((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
	float uB = (float)((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));

	*ix = x1 + (uA * (x2-x1));
	*iy = y1 + (uA * (y2-y1));

	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
		return 1;

	return 0;
}

int polytopoint(poly *a, int px, int py, int *ix, int *iy) {
	int result = 0;
	int current, next;
	int vcx, vnx, vcy, vny;

	next = 0;

	for (current = 0; current <= a->vc*2; current += 2) {
		next = current + 2;

		if (next == a->vc*2)
			next = 0;

		vcx = a->v[current] + a->x;
		vnx = a->v[next] + a->y;
		vcy = a->v[current+1] + a->x;
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

int collontilemap(poly *p, tmap *t, int *rx, int *ry) {
	if (p->x + p->w < t->x) return 0;
	if (p->y + p->h < t->y) return 0;
	for (int i=0; i < p->vc; i++) {
		int x = (stepify(p->x + p->v[i], t->cellsize)-t->x)/t->cellsize;
		int y = (stepify(p->y + p->v[i+1], t->cellsize)-t->y)/t->cellsize;
		if (x < 0) continue;
		if (y < 0) continue;
		if (x > t->w) continue;
		if (y > t->h) continue;
		if (t->cells[y*t->w+x] <= 0) continue;
		if (t->collmask[t->cells[y*t->w + x]-1]) {
			*rx = x;
			*ry = y;
			return 1;
		}
	}
	return 0;
}
