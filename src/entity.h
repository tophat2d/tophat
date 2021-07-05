#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>
#include "rect.h"
#include "polygon.h"
#include "image.h"

#pragma pack(push, 1)
typedef struct {
	th_poly p;
	th_image *img;
	double sx;
	double sy;
	int rot;
	uint32_t color;
	int id;
} th_ent;
#pragma pack(pop)

void th_ent_draw(th_ent *o, th_rect *camera);
int th_ent_getcoll(th_ent *e, th_ent *scene, int count, int *ix, int *iy);

#endif
