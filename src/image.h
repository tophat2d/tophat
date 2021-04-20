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

image *loadimage(char *path);

void flipv(image *img);

void fliph(image *img);

void imgcrop(image *img, int x1, int y1, int x2, int y2);

void rdimg(image *img, unsigned char *data);

#endif
