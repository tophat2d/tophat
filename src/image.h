#ifndef IMAGE_H
#define IMAGE_H

typedef struct {
	unsigned char *raw;
	int w;
	int h;
	int c;
	uint32_t *rdimg;
} image;

image *loadimage(char *path);

void testdraw(image *img);

void rdimg(image *img, float scaling);

#endif
