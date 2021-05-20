#ifndef LIGHT_H
#define LIGHT_H

#include "rect.h"
#include <stdint.h>

typedef struct {
	int px;
	int py;
	int rotation;
	int lenght;
	int width;
	uint32_t color;
} th_lightcone;

void th_draw_lightcone(th_lightcone *l, th_rect *cam);

#endif
