#include "entity.h"
#include <stdio.h>

#include "collisions.h"
#include "main.h"
#include "poly.h"

extern int entitycount;

int collbyentity(entnode_t *a, entity *e) {
	entnode_t *current, *next;
	int result;	
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

int polytopoly(poly *a, poly *b) {
	int result = 0;	
	int current, next;
	int px, py;
	int vcx, vnx, vcy, vny;
	for (int i=0; i <= b->vc*2; i += 2) {
		px = b->v[i] + b->x;
		py = b->v[i+1] + b->y;
		for (current = 0; current <= a->vc*2; current += 2) {
			next = current + 2;
  
			if (next >= a->vc*2) {
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
		if (result) {
			return result;
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

