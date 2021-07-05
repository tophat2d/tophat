#include "raycast.h"
#include "collisions.h"
#include "entity.h"
#include "rect.h"
#include "misc.h"

int th_ray_getcoll(th_ray *ra, th_ent *scene, int count, int *ix, int *iy) {
	float rx, ry;
	rx = ra->x;
	ry = ra->y - ra->l;
	int coll;

	th_rotate_point(&rx, &ry, ra->x, ra->y, ra->r);
	th_rect rr = (th_rect){ra->x, ra->y, rx, ry};

	if (rx < ra->x) {
		rr.x = rx;
		rr.w = ra->x;
	}

	if (ry < ra->y) {
		rr.y = ry;
		rr.h = ra->y;
	}

	for (int i=0; i < count; i++) {
		if (rr.x > scene[i].p.x + scene[i].p.w) continue;
		if (rr.y > scene[i].p.y + scene[i].p.h) continue;
		if (rr.w < scene[i].p.x) continue;
		if (rr.h < scene[i].p.y) continue;

		coll = _th_poly_to_line(&scene[i].p, ra->x, ra->y, rx, ry, ix, iy);
		if (coll)
			return scene[i].id;

		coll = _th_poly_to_point(&scene[i].p, rx, ry, ix, iy);
		if (coll)
			return scene[i].id;
	}
	return 0;
}
