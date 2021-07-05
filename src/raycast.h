#ifndef RAY_H
#define RAY_H

#include "entity.h"

typedef struct {
	int x;
	int y;
	int l;
	int r;
} th_ray;

int th_ray_getcoll(th_ray *ra, th_ent *scene, int count, int *ix, int *iy);

#endif
