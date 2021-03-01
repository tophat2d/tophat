#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>

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
	int coll = 0;	
	struct timeval start, stop;
	double delta;

	char *fpstext = malloc(sizeof(char) * 40);

	short w, h;

	entity o = entityfrompoly(newpoly(10, 50, 4, 0, 0, 20, 0, 20, 20, 0, 20), 0xffffebff);
	entity p = entityfrompoly(newpoly(60, 20, 4, 0, 0, 1, 0, 41, 20, 40, 20), 0xffffebff);
	entity player = entityfrompoly(newpoly(0, 0, 6, 0, 0, 10, 20, 20, 20, 30, 10, 20, 0, 10, 0), 0x22ff22ff);
	rect cam = newrect(20, 70, 210, 120);

	addentity(&entities, &o);
	addentity(&entities, &player);
	addentity(&entities, &p);

	CNFGSetup( "Example App", 1024, 768 );

	CNFGGetDimensions(&w, &h);
	scaling =	getscaling(w, h, cam.w, cam.h);

	while (1) {
		gettimeofday(&start, NULL);

		CNFGBGColor = 0x080808ff;

		CNFGClearFrame();
		CNFGHandleInput();
		CNFGGetDimensions(&w, &h);
		scaling = getscaling(w, h, cam.w, cam.h); 


		player.p->x = mx/scaling + cam.x - cam.w/2;
		player.p->y = my/scaling + cam.y - cam.h/2;

		visualizecam(cam);

		CNFGColor(0xffffffff);
		CNFGPenX = 1;
		CNFGPenY = 1;
		sprintf(fpstext, "fps: %d", (float)1/delta);
		printf("fps: %d\n", (float)1/delta);
		CNFGDrawText(fpstext, 2);

		draw(o, cam);
		draw(player, cam);
		draw(p, cam);

		coll = collbyentity(&entities, &player);
		if (coll != 0) {
			player.color = 0xffff00ff;
		} else {
			player.color = 0x22ff22ff;
		}
		coll = 0;

		CNFGSwapBuffers();

		gettimeofday(&stop, NULL);
		delta = (double)(stop.tv_usec - start.tv_usec) / 1000 + (double)(stop.tv_sec - start.tv_sec);;
	}


	free(fpstext);
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

	int len = 0;

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
		len++;
	}
	printf("lenght %d\n", len);
	entnode_t *ta = malloc(sizeof(entnode_t));
	ta->next = NULL;
	ta->val = e;
	current->next = ta;
}
