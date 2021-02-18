#ifndef OBJECT_H
#define OBJECT_H

#include "rect.h"

typedef struct {
	rect r;
	int c;
	char *image;
	uint32_t color;
} entity;

entity entityfromrect(rect r, uint32_t color);
entity entityfromimage(char path[]);
void draw(entity o);

#endif
