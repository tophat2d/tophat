#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb/stb_image.h"

#include "../lib/rawdraw/CNFG.h"

#include <GL/gl.h>

#include "image.h"

image *loadimage(char *path) {
	int w, h, c;

	unsigned char *data = stbi_load(path, &w, &h, &c, 0);

	image *img;
	img = malloc(sizeof(image));
	img->w = w;
	img->h = h;
	img->c = c;
	img->raw = data;

	return img;
}

void imgslowdraw(image *img) {
	for (int y=0; y < img->h / img->c; y++) {
		for (int x=0; x < img->w / img->c; x++) {
			if (img->rdimg[(y * img->w) + x] != 1) {
				CNFGColor(img->rdimg[(y * img->w) + x] * 256 + 255);
				CNFGTackPixel(x, y);
			}	
		}
	}
}

void flipv(image *img) {
	uint32_t *f;
	f = malloc(sizeof(uint32_t) * img->w * img->h);

	for (int i=0; i < img->w; i++) {
		for (int j=0; j < img->h; j++) {
			f[(j + 1) * img->w - i] = img->rdimg[j * img->w + i];
		}
	}

	free(img->rdimg);
	img->rdimg = f;
}

void fliph(image *img) {
	uint32_t *f;
	f = malloc(sizeof(uint32_t) * img->w * img->h);

	for (int i=0; i < img->w; i++) {
		for (int j=0; j < img->h; j++) {
			f[(img->h - j - 1) * img->w + i] = img->rdimg[j * img->w + i];
		}
	}

	free(img->rdimg);
	img->rdimg = f;
}

void rdimg(image *img, float scaling) {

	uint32_t *rd;
	rd = malloc(sizeof(int) * img->w * img->h);
	uint32_t current = 0;

	for (int y=0; y < img->h; y += 1) {
		for (int x=0; x < img->w; x += 1) {
			current = 0;
			for (int i=0; i < img->c; i++) {
				current *= 256;
				if (i > 3) {
					current += 255;
				} else {
					current += (uint32_t)img->raw[(y * img->w + x) * img->c + i];
				}
			}

			for (int i=0; i < 4 - img->c; i++) {
				if (current == 1) {
					current = 0x00 | current<<8;
					continue;
				}

				current = 0xff | current<<8;
			}
			if (current < 0) {
				printf("%x\n", current);
			}
			rd[(y * img->w + x)] = current;
		}
	}

	/*
	uint32_t *rd;
	rd = malloc(sizeof(int) * img->w * img->h);
	uint32_t current = 0;
	float errorx, errory;


	for (int y=0; y < img->h; y += img->c) {
		for (int j=0; j < (int)scaling; j++) {
			errory += scaling - (int)scaling;

			for (int x=0; x < img->w; x += img->c) {
				errorx += scaling - (int)scaling;
      
				for (int i=0; i < img->c; i++) {
					current *= 255;
					if (i > 3) {
						current += 255;
					} else {
						current += (uint32_t)img->raw[(y*img->h + x)+i];
					}
				}
     
				printf("current: %X\n", current);

				for (int i=0; i < (int)scaling; i++) {
					rd[(y*img->h + x)/img->c + i] = current;
				}
      
				if (errorx >= 1) {
					rd[(y*img->h + x)/img->c + (int)scaling] = current;
					errorx -= 1;
				}
			}

			if (errory >= 1) {
				j--;
				errory -= 1;
			}

		}
	}*/

	img->rdimg = rd;

}
