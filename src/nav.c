#include "tophat.h"
#include <umka_api.h>

#include <math.h>
#include <stdlib.h>

static
th_vf2 vf2_to_loc(th_navmesh *m, th_vf2 p) {
	p.x -= m->r.x;
	p.y -= m->r.y;

	p.x = trunc(p.x / m->s);
	p.y = trunc(p.y / m->s);

	return p;
}

void th_navmesh_add_quad(th_navmesh *m, th_quad *q) {
	for (uu i = 0; i < 4; ++i)
		q->v[i] = vf2_to_loc(m, q->v[i]);

	th_rect r = th_quad_bounding_box(*q);

	th_vf2 ic = {0};
	// NOTE(~mrms): this is slow and could be implemented better with some magic,
	// but it will work for now.
	for (fu x = r.x + 0.5f; x < r.x + r.w; ++x)
		for (fu y = r.y + 0.5f; y < r.y + r.h; ++y)
			if (th_point_to_quad((th_vf2){.x=x, .y=y}, q, &ic))
				m->d.data[(uu)x + (uu)y*m->w] = 0;
}

// euclidian distance
static inline
fu heuristic(th_vf2 p1, th_vf2 p2) {
	return pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2);
}

struct qnode {
	th_vf2 v;
	struct qnode *next;
};

static
struct qnode *push(struct qnode *q, th_vf2 p, fu *hcost, uu w) {
	if (q != NULL && q->v.x == p.x && q->v.y == p.y)
		return q;

	if (q == NULL || hcost[(uu)(p.x + p.y*w)] < hcost[(uu)(q->v.x + q->v.y*w)]) {
		struct qnode *nq = malloc(sizeof(struct qnode));
		nq->next = q;
		nq->v = p;
  
		return nq;
	}

	q->next = push(q->next, p, hcost, w);
	return q;
}

static inline
struct qnode *pop(struct qnode *q) {
	if (q == NULL)
		return NULL;

	struct qnode *nq = q->next;
	free(q);

	return nq;
}

static inline
bool check_bounds(th_navmesh *m, th_vf2 p, size_t h) {
	return p.x >= 0
		&& p.y >= 0
		&& p.x < m->w
		&& p.y < h;
}

void th_navmesh_nav(th_vf2 *cameFrom, th_navmesh *m, th_vf2 p1, th_vf2 p2) {
	const th_vf2 movemap[] = {
		{{-1, -1}}, {{+0, -1}}, {{+1, -1}},
		{{-1, +0}},             {{+1, +0}},
		{{-1, +1}}, {{+0, +1}}, {{+1, +1}}
	};
	const size_t msiz = umkaGetDynArrayLen((void *)&m->d);
	const size_t mh = msiz / m->w;

	p1 = vf2_to_loc(m, p1);
	p2 = vf2_to_loc(m, p2);
	
	struct qnode *q = push(NULL, p1, NULL, 0);
	/*th_vf2 *cameFrom = calloc(sizeof(th_vf2), umkaGetDynArrayLen((void *)&m->d));
	for (int i=0; i < umkaGetDynArrayLen((void *)&m->d); ++i) {
		cameFrom[i].x = -1;
		cameFrom[i].y = -1;
	}*/

	fu *cost = calloc(sizeof(fu), msiz);
	for (int i=0; i < msiz; ++i) {
		cost[i] = -1;
	}
	cost[(uu)(p1.x + p1.y*m->w)] = 0;

	fu *hcost = calloc(sizeof(fu), msiz);
	hcost[(uu)(p1.x + p2.y*m->w)] = heuristic(p1, p2);

	while (q) {
		th_vf2 p = q->v;

		if (p.x == p2.x && p.y == p2.y)
			break;

		q = pop(q);
		for (int i=0; i < 8; ++i) {
			th_vf2 nb = p;
			nb.x -= movemap[i].x;
			nb.y -= movemap[i].y;
			const uu idx = nb.x + nb.y*m->w;

			// skip out of bounds fields
			if (!check_bounds(m, nb, mh))
				continue;

			// skip inacessible fields
			if (!m->d.data[(uu)(nb.x + nb.y*m->w)])
				continue;

			fu c = cost[(uu)(p.x + p.y*m->w)] + 1;
			if (cost[idx] < c && cost[idx] != -1)
				continue;

			cost[idx] = c;
			hcost[idx] = c + heuristic(nb, p2);
			if (cameFrom[idx].x < 0)
				q = push(q, nb, hcost, m->w);
			cameFrom[idx] = p;
		}
	}

	free(hcost);
	free(cost);
}