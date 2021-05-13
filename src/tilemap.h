#ifndef TILEMAP_H
#define TILEMAP_H

#include "image.h"
#include "rect.h"

typedef struct {
	image **tiles;
	int x;
	int y;
	int w;
	int h;
	int *cells;
	int *collmask;
	int cellsize;
	int scaletype;
} tmap;

void tmapdraw(tmap *t, rect *cam);

#endif
