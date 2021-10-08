#include <stdio.h>
#include <stdlib.h>
#include <CNFG.h>

#include "tophat.h"

extern th_global thg;

th_quad th_ent_transform(th_ent *e) {
	th_quad q = {0};

	q.tr.x = e->rect.w;
	q.br = (th_vf2){{e->rect.w, e->rect.h}};
	q.bl.y = e->rect.h;

	for (uu i=0; i < 4; i++) {
		q.v[i].x *= e->t.scale.x;
		q.v[i].y *= e->t.scale.y;
		th_rotate_point(&q.v[i], e->t.origin, e->t.rot);
		q.v[i].x += e->t.pos.x;
		q.v[i].y += e->t.pos.y;
	}

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
		r = (th_rect){.w = o->img->dm.w, .h = o->img->dm.h};
	th_quad q = th_ent_transform(&(th_ent){.rect = r, .t = t});
	
	// this logic is incorrect
	if (q.br.x < 0 || q.br.y < 0)
		return;

	if (q.tl.x > camera->w || q.tl.y > camera->h)
		return;

	for (uu i=0; i < 4; i++) {
		q.v[i].x *= thg.scaling;
		q.v[i].y *= thg.scaling;
	}

	if (!o->img) {
		RDPoint verts[4];
		for (uu i=0; i < 4; i++) {
			verts[i].x = q.v[i].x;
			verts[i].y = q.v[i].y;
		}

		CNFGColor(o->color);
		CNFGTackPoly(&verts[0], 4);
		return;
	}

	th_blit_tex(o->img->gltexture, q);
}

iu th_ent_getcoll(th_ent *e, th_ent **scene, uu count, th_vf2 *ic) {
	for (int i=0; i < count; i++) {
		if (e == scene[i]) continue;

		if (th_ent_to_ent(scene[i], e, ic))
			return i;
	}
	return -1;
}
