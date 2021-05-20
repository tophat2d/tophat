#ifndef TILEMAP_H
#define TILEMAP_H

#include "image.h"
#include "rect.h"

typedef struct {
	th_image **tiles;
	int x;
	int y;
	int w;
	int h;
	int *cells;
	int *collmask;
	int cellsize;
	int scaletype;
} th_tmap;

void th_tmap_draw(th_tmap *t, th_rect *cam);

#endif
