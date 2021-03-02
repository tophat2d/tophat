//
// largely inpired by (copied from :]) this guide: http://jeffreythompson.org/collision-detection/table_of_contents.php
//
#include "entity.h"
#include <stdio.h>

#include "collisions.h"
#include "main.h"
#include "poly.h"

extern int entitycount;

int collbyentity(entnode_t *a, entity *e) {
	entnode_t *current, *next;
	int i = 0;
	int coll;

	current = a;
	next = a;

	if (e == NULL) {
		return 1;
	}

	i = 0;

	while (next != NULL) {

		current = next;
		next = current->next;

		i++;

		if (current->val == NULL) {
			current = current->next;
			continue;
		}

		if (e->id == current->val->id) {
			continue;
		}

		coll = polytopoly(current->val->p, e->p);
		if (coll) {
			return current->val->id;
		}

		if (current->next == NULL) {
			return 0;
		}
	}
	return 0;
}

int polytopoly(poly *p1, poly *p2) {
	int next = 0;
	int vcx, vcy, vnx, vny;
	int coll = 0;

	for (int current = 0; current < p1->vc * 2; current += 2) {
		next = current + 2;

		if (next >= p1->vc) {
			next = 0;
		}

		vcx = p1->v[current] + p1->x;
		vcy = p1->v[current + 1] + p1->y;
		vnx = p1->v[next] + p1->x;
		vny = p1->v[next + 1] + p1->y;

		coll = polytoline(p2, vcx, vcy, vnx, vny);
		if (coll) {
			return 1;
		}

		coll = polytopoint(p1, p2->v[0], p2->v[1]);
		if (coll) {
			return 1;
		}
	}

	return 0;
}

int polytoline(poly *a, int sx, int sy, int ex, int ey) {
	int next = 0;
	int csx, csy, cex, cey;
	int coll = 0;

	for (int current = 0; current < a->vc*2; current += 2) {
		next = current + 2;

		csx = a->v[current] + a->x;
		csy = a->v[current + 1] + a->y;
		cex = a->v[next] + a->x;
		cey = a->v[next + 1] + a->y;

		coll = linetoline(sx, sy, ex, ey, csx, csy, cex, cey);
		if (coll) {
			return 1;
		}
	}

	return 0;
}

int linetoline(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
	float uA = (float)((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
	float uB = (float)((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));

	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
		return 1;
	}

	return 0;
}

int polytopoint(poly *a, int px, int py) {
	int result = 0;
	int current, next;
	int vcx, vnx, vcy, vny;

	next = 0;

	for (current = 0; current <= a->vc*2; current += 2) {
		next = current + 2;

		if (next == a->vc*2) {
			next = 0;
		}

		vcx = a->v[current] + a->x;
		vnx = a->v[next] + a->y;
		vcy = a->v[current+1] + a->x;
		vny = a->v[next+1] + a->y;

		// this is some kind of black magic i found on the internet.
		if (((vcy >= py && vny < py) || (vcy < py && vny >= py)) && (px < (vnx-vcx)*(py-vcy) / (vny-vcy)+vcx)) {
			result = !result;
		}
	}
	return result;
}

/*
   int collbyentity(entnode_t *a, entity *e) {
   int id, aid, x, y, w, h, ax, ay, aw, ah;
   entnode_t *current, *next;

   id = e->id;
   x = e->r.x;
   y = e->r.y;
   w = e->r.w;
   h = e->r.h;

   next = a;
   current = a;

   while (next != NULL) {

   if (current->val == NULL) {
   next = current->next;
   current = next;
   continue;
   }

   aid = current->val->id;
   ax = current->val->r.x;
   ay = current->val->r.y;
   ah = current->val->r.h;
   aw = current->val->r.w;

   next = current->next;
   current = next;

   if (id == aid) {
   continue;
   }

   if (x + w <= ax) {
   continue;
   }

   if (y + h <= ay) {
   continue;
   }

   if (x >= ax+aw) {
   continue;
   }

   if (y >= ay+ah) {
   continue;
   }

   return aid;
   }

   return 0;
   }*/

