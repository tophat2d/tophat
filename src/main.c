#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "../lib/rawdraw/CNFG.h"
#include "entity.h"
#include "debug.h"
#include "misc.h"
#include "main.h"
#include "collisions.h"
#include "poly.h"

int mx, my;

void HandleKey( int keycode, int bDown ) { }
void HandleButton( int x, int y, int button, int bDown ) { }
void HandleMotion( int x, int y, int mask ) { 
	mx = x;
	my = y;
}
void HandleDestroy() { }

int scaling;
entnode_t entities;
int entitycount;
// this will be a init function or something
int main() {
	srand(time(NULL));
	int coll;	


	short w, h;

	entity o = entityfrompoly(newpoly(10, 20, 4, 0, 0, 20, 0, 20, 20, 0, 20), 0xffffebff);
	entity player = entityfrompoly(newpoly(0, 0, 6, 0, 0, 10, 20, 20, 20, 30, 10, 20, 0, 10, 0), 0x22ff22ff);
	rect cam = newrect(20, 70, 210, 120);

	addentity(&entities, &o);
	addentity(&entities, &player);

	CNFGSetup( "Example App", 1024, 768 );

	CNFGGetDimensions(&w, &h);
	scaling =	getscaling(w, h, cam.w, cam.h);

	while (1) {
		CNFGBGColor = 0x080808ff;

		CNFGClearFrame();
		CNFGHandleInput();
		CNFGGetDimensions(&w, &h);
		scaling = getscaling(w, h, cam.w, cam.h); 

		player.p->x = mx/scaling + cam.x - cam.w/2;
		player.p->y = my/scaling + cam.y - cam.h/2;

		visualizecam(cam);
		draw(o, cam);
		draw(player, cam);

		coll = collbyentity(&entities, &player);
		if (coll != 0) {
			player.color = 0xffff00ff;
		} else if (coll == 1) {
			player.color = 0xff2200ff;
		} else {
			player.color = 0x22ff22ff;
		}

		CNFGSwapBuffers();

	}



	freeentnodes(&entities);
	return 0;
}

void freeentnodes(entnode_t *s) {
	entnode_t *next, *current;
	next = s;

	while (next != NULL) {
		current = next;
		next = current->next;
		free(current->val->p->v);
		free(current->val->p);
		free(current);
	}
}

void addentity(entnode_t *s, entity *e) {

	entnode_t *next, *current;
	next = s;

	if (next == NULL) {
		entnode_t ta;
		ta.next == NULL;
		ta.val = e;
		s = &ta;

		return;
	}

	while (next != NULL) {
		current = next;
		next = current->next;
	}

	entnode_t *ta = malloc(sizeof(entnode_t));
	ta->next = NULL;
	ta->val = e;
	current->next = ta;
}
