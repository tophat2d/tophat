#ifndef IMAGE_H
#define IMAGE_H

typedef struct {
	unsigned char *raw;
	int w;
	int h;
	int c;
	float scalex;
	float scaley;
	uint32_t *rdimg;
	unsigned int tex;
} image;

image *loadimage(char *path);

void imgslowdraw(image *img);

void rdimg(image *img, float scaling);

#endif
