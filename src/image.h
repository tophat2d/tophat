#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

typedef struct {
	int w;
	int h;
	int c;
	uint32_t *rdimg;
	unsigned int tex;
} image;

void blittex(unsigned int tex, int x, int y, int w, int h, float rot);

void imagefromdata(image *img, uint32_t *data, int w, int h);

image *loadimage(char *path);

void flipv(image *img);

void fliph(image *img);

void imgcrop(image *img, int x1, int y1, int x2, int y2);

void rdimg(image *img, unsigned char *data);

#endif
