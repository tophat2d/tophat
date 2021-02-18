#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb/stb_image.h"

#include <stdio.h>

#define CNFG_IMPLEMENTATION
#include "../lib/rawdraw/CNFG.h"

#include "entity.h"
#include "rect.h"

entity entityfromrect(rect r, uint32_t color) {
	entity e;
	e.r = r;
	e.color = color;
	e.image = NULL;

	return e;
}

entity entityfromimage(char path[]) {
	int w, h, c;

	unsigned char *data = stbi_load(path, &w, &h, &c, 3);

	if (data == NULL) {
		printf("error loading data");
		exit(1);
	}

	entity tr;
	tr.image = data;
	tr.r.w = w;
	tr.r.h = h;
	tr.c = c;

	stbi_image_free(data);

	return tr;
}

void draw(entity o/*, cam camera*/) {
	if (o.image == NULL) {
		CNFGColor(o.color);
		CNFGTackRectangle(o.r.x, o.r.y, o.r.w + o.r.x, o.r.h + o.r.y);

		return;
	}


	size_t img_size = o.r.w * o.r.h * o.c;
	unsigned char *img = o.image;
	int i;
	short x, y;
	uint32_t color = 0;
	uint32_t multiplier = 1000000000;

	for (unsigned char *p = img; p != img + img_size; p += o.c) {
		if (y >= o.r.w) {
			x++;
			y = 0;
		}
		for (int tmp=0; tmp < o.c; tmp++) {
			color += multiplier * (uint32_t)*(p + tmp);
			printf("%d * %d\n", multiplier, *(p + tmp));
			multiplier /= 1000;
		}
	
		y++;
		//CNFGColor(color);
		//CNFGTackPixel(x, y);
		if (color != 0) {
			printf("color: %d, x: %d, y: %d\n", color, x, y);
		}

		i++;
	}
}
