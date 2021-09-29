#include "tophat.h"

int th_ray_getcoll(th_ray *ra, th_ent *scene, int count, int *ix, int *iy) {
	float rx, ry;
	rx = ra->pos.x;
	ry = ra->pos.y - ra->l;
	int coll;

	th_rotate_point(&rx, &ry, ra->pos.x, ra->pos.y, ra->r);
	th_rect rr = (th_rect){ra->pos.x, ra->pos.y, rx, ry};

	if (rx < ra->pos.x) {
		rr.x = rx;
		rr.w = ra->pos.x;
	}

	if (ry < ra->pos.y) {
		rr.y = ry;
		rr.h = ra->pos.y;
	}

	for (int i=0; i < count; i++) {
		if (rr.x > scene[i].p.x + scene[i].p.w) continue;
		if (rr.y > scene[i].p.y + scene[i].p.h) continue;
		if (rr.w < scene[i].p.x) continue;
		if (rr.h < scene[i].p.y) continue;

		coll = _th_poly_to_line(&scene[i].p, ra->pos.x, ra->pos.y, rx, ry, ix, iy);
		if (coll)
			return scene[i].id;

		coll = _th_poly_to_point(&scene[i].p, rx, ry, ix, iy);
		if (coll)
			return scene[i].id;
	}
	return 0;
}
