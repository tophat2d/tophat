#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>
#include "rect.h"
#include "poly.h"
#include "image.h"

typedef struct {
	poly *p;
	image *img;
	double sx;
	double sy;
	int rot;
	uint32_t color;
	int id;
} entity;

typedef struct entnode {
	entity *val;
	struct entnode *next;
} entnode_t;

void draw(entity *o, rect *cam);

#endif
