#ifndef LIGHT_H
#define LIGHT_H

#include "rect.h"

typedef struct {
	uint32_t *dots;
	int w, h;
	double rect_size;
	uint32_t color;
} th_lightmask;

typedef struct {
	int x, y;
	int radius;
	uint32_t tint;
} th_spotlight;

void th_lightmask_clear(th_lightmask *d);
void th_lightmask_draw(th_lightmask *d, th_rect *cam);
void _th_lightmask_stamp_point(th_lightmask *d, int x, int y, uint32_t color);
void th_spotlight_stamp(th_spotlight *l, th_lightmask *d);

#endif
