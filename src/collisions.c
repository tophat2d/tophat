#include "entity.h"
#include "collisions.h"
#include "main.h"

#include <stdio.h>

extern int entitycount;

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

