
#include "tophat.h"

void th_transform_rect(th_quad *q, th_transform t, th_rect r) {
	q->tl.x = r.x;
	q->tl.y = r.y;
	q->tr.x = r.x + r.w;
	q->tr.y = r.y;
	q->br.x = r.x + r.w;
	q->br.y = r.y + r.h;
	q->bl.x = r.x;
	q->bl.y = r.y + r.h;

	for (uu i=0; i < 4; i++) {
		q->v[i].x *= t.scale.x;
		q->v[i].y *= t.scale.y;
		th_rotate_point(&q->v[i], t.origin, t.rot);
		q->v[i].x += t.pos.x;
		q->v[i].y += t.pos.y;
	}
}
