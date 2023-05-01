#include <stdio.h>
#include <stdlib.h>

#include "tophat.h"

extern th_global *thg;

th_quad th_ent_transform(th_ent *e) {
	th_quad q;
	th_transform_rect(&q, e->t, e->rect);

	return q;
}

void th_ent_draw(th_ent *o) {
	th_transform t = o->t;

	if (!o->img) {
		th_quad q;
		th_transform_rect(&q, t, o->rect);
		th_canvas_triangle(o->color, q.v[0], q.v[1], q.v[2]);
		th_canvas_triangle(o->color, q.v[0], q.v[2], q.v[3]);
		return;
	}

	th_image_render_transformed(o->img, t, o->color);
}

void th_ent_getcoll(th_ent *e, th_ent **scene, uu count, uu *collC,
	uu maxColls, th_coll *colls) {

	for (int i=0; i < count && *collC < maxColls; i++) {
		if (e == scene[i]) continue;

		if (th_ent_to_ent(scene[i], e, &colls[*collC].pos)) {
			colls[(*collC)++].index = i;
		}
	}
}
