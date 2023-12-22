#include "tophat.h"

th_vf2
th_quad_min(th_quad q)
{
	th_vf2 p;
	th_vf2 set = {0};

	for (uu i = 0; i < 4; i++) {
		if (!set.x || q.v[i].x < p.x) {
			set.x = 1;
			p.x = q.v[i].x;
		}

		if (!set.y || q.v[i].y < p.y) {
			set.y = 1;
			p.y = q.v[i].y;
		}
	}

	return p;
}

th_vf2
th_quad_max(th_quad q)
{
	th_vf2 p;
	th_vf2 set = {0};

	for (uu i = 0; i < 4; i++) {
		if (!set.x || q.v[i].x > p.x) {
			set.x = 1;
			p.x = q.v[i].x;
		}

		if (!set.y || q.v[i].y > p.y) {
			set.y = 1;
			p.y = q.v[i].y;
		}
	}

	return p;
}

th_rect
th_quad_bounding_box(th_quad q)
{
	th_vf2 min = th_quad_min(q);
	th_vf2 max = th_quad_max(q);

	return (th_rect){min.x, min.y, max.x - min.x, max.y - min.y};
}
