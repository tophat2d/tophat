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
} lightcone;

void drawlightcone(lightcone *l, rect *cam);

#endif
