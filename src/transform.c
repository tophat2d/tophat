#include "tophat.h"

void
th_transform_quad(th_quad *q, th_transform t)
{
	for (uu i = 0; i < 4; i++) {
		q->v[i].x *= t.scale.x;
		q->v[i].y *= t.scale.y;
		th_rotate_point(&q->v[i], t.origin, t.rot);
		q->v[i].x += t.pos.x;
		q->v[i].y += t.pos.y;
	}
}

void
th_transform_rect(th_quad *q, th_transform t, th_rect r)
{
	q->tl.x = r.x;
	q->tl.y = r.y;
	q->tr.x = r.x + r.w;
	q->tr.y = r.y;
	q->br.x = r.x + r.w;
	q->br.y = r.y + r.h;
	q->bl.x = r.x;
	q->bl.y = r.y + r.h;

	th_transform_quad(q, t);
}

void
th_transform_vf2(th_vf2 *v, th_transform t)
{
	v->x *= t.scale.x;
	v->y *= t.scale.y;
	th_rotate_point(v, t.origin, t.rot);
	v->x += t.pos.x;
	v->y += t.pos.y;
}

void
th_transform_transform(th_transform *o, th_transform t)
{
	th_vf2 to = t.origin;
	to.x += t.pos.x;
	to.y += t.pos.y;

	th_transform_vf2(&o->pos, t);
	o->scale.x *= t.scale.x;
	o->scale.y *= t.scale.y;
	th_transform_vf2(&o->origin, t);
	o->rot += t.rot;
}

void
th_transform_inverse(th_transform *out, th_transform t)
{
	fu sx_inv = t.scale.x != 0 ? 1.0 / t.scale.x : 0.0;
	fu sy_inv = t.scale.y != 0 ? 1.0 / t.scale.y : 0.0;

	th_vf2 ro = t.origin;
	th_rotate_point(&ro, (th_vf2){{0, 0}}, t.rot);

	th_vf2 c;
	c.x = t.origin.x - ro.x + t.pos.x;
	c.y = t.origin.y - ro.y + t.pos.y;

	th_vf2 r_inv_c = c;
	th_rotate_point(&r_inv_c, (th_vf2){{0, 0}}, -t.rot);

	out->pos.x = -r_inv_c.x * sx_inv;
	out->pos.y = -r_inv_c.y * sy_inv;
	out->scale.x = sx_inv;
	out->scale.y = sy_inv;
	out->origin = (th_vf2){{0, 0}};
	out->rot = -t.rot;
}
