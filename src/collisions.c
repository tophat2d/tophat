#include "entity.h"
#include "collisions.h"
#include "main.h"

#include <stdio.h>

extern int entitycount;

int collbyentity(entnode_t *a, entity *e) {
	entnode_t *next, *current;
	int result;	

	next = a;
	current = a;

	while (next != NULL) {

		if (current->val == NULL) {
			next = current->next;
			current = next;
			continue;
		}

		next = current->next;
		current = next;

		if (id == aid) {
			continue;
		}

		if ((result = polytopoly(a->p, current->p)) != 0) {
			return result;
		}
	}
}

int polytopoly(poly *a, poly *b) {
	
	int current, next;
	int px, py;
	int vcx, vnx, vcy, vny;
	for (int i=0; i < b->vc*2; i += 2) {
		px = b->v[i];
		py = b->v[i+1];
		for (current = 0; current < a->vc*2; current += 2) {
			next = current + 2;
  
			if (next >= a->vc*2) {
				next = 0;
			}
  
			vcx = a->v[current];
			vnx = a->v[next];
			vcy = a->v[current+1];
			vny = a->v[next+1];
			
			// this is some kind of black magic i found on the internet. idk how it works, but it does the job
			if (((vcy >= py && vny < py) || (vcy < py && vny >= py)) && (px < (vnx-vcx)*(py-vcy) / (vny-vcy)+vcx)) {
				return b->id;
			}
		}
	}
}


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
}

