#ifndef IMAGE_H
#define IMAGE_H

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

void rdimg(image *img, unsigned char *data);

#endif
