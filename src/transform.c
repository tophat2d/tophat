
#include "tophat.h"

void th_transform_rect(th_quad *q, th_transform t, th_rect r) {
	q->tr.x = r.w;
	q->br.x = r.w;
	q->br.y = r.h;
	q->bl.y = r.h;

	for (uu i=0; i < 4; i++) {
		q->v[i].x *= t.scale.x;
		q->v[i].y *= t.scale.y;
		th_rotate_point(&q->v[i], t.origin, t.rot);
		q->v[i].x += t.pos.x;
		q->v[i].y += t.pos.y;
	}
}
