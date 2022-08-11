#include <stdio.h>
#include <stdlib.h>

#include "tophat.h"

extern th_global thg;

th_quad th_ent_transform(th_ent *e) {
	th_quad q;
	th_transform_rect(&q, e->t, e->rect);

	return q;
}

void th_ent_draw(th_ent *o, th_rect *camera) {
	int camx, camy;
	camx = camera->x - (camera->w / 2);
	camy = camera->y - (camera->h / 2);
	th_transform t = o->t;
	t.pos.x -= camx;
	t.pos.y -= camy;

	th_rect r = o->rect;
	if (o->img)
		r = (th_rect){
		.w = (o->img->crop.w - o->img->crop.x) * o->img->dm.x,
		.h = (o->img->crop.h - o->img->crop.y) * o->img->dm.y};
	th_quad q = {0};
	th_transform_rect(&q, t, r);
	
	// this logic is incorrect
	/*if (q.br.x < 0 || q.br.y < 0)
		return;

	if (q.tl.x > camera->w || q.tl.y > camera->h)
		return;*/

	if (!o->img) {
		for (uu i=0; i < 4; i++) {
			q.v[i].x *= thg.scaling;
			q.v[i].y *= thg.scaling;
		}
		th_canvas_triangle(o->color, q.v[0], q.v[1], q.v[2]);
		th_canvas_triangle(o->color, q.v[0], q.v[2], q.v[3]);
		return;
	}

	th_blit_tex(o->img, q, o->color);
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
