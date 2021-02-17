#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb/stb_image.h"

#include <stdio.h>

#define CNFG_IMPLEMENTATION
#include "../lib/rawdraw/CNFG.h"

#include "object.h"

obj load(char path[]) {
	int w, h, c;

	unsigned char *data = stbi_load(path, &w, &h, &c, 3);

	if (data == NULL) {
		printf("error loading data");
		exit(1);
	}

	uint32_t image[w*h]

	size_t img_size = w * h * 3;

	for (unsigned char *p = data; p != data + img_size; p += 3) {
		image[]
	}
	

	obj tr;
	tr.image = data;
	tr.w = w;
	tr.h = h;

	stbi_image_free(data);

	return tr;
}

void draw(obj o/*, cam camera*/) {
/*	if o.x < cam.sx || o.x > cam.ex {
			return;
	}
	if o.y < cam.sy || o.y > cam.ey {
			return;
	}


	CNFGBlitImage(o.image, o.px-cam.sx, o.py-cam.sy, o.w, o.h);*/

	

	//CNFGBlitImage(&o.image, o.px, o.py, o.w, o.h);
}

