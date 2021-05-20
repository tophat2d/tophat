#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

typedef struct {
	int w;
	int h;
	int c;
	uint32_t *rdimg;
	unsigned int tex;
} th_image;

th_image *th_load_image(char *path);
void th_free_image(th_image *img);
void th_image_from_data(th_image *img, uint32_t *data, int w, int h);
void th_blit_tex(unsigned int tex, int x, int y, int w, int h, float rot);
void th_image_flipv(th_image *img);
void th_image_fliph(th_image *img);
void th_image_crop(th_image *img, int x1, int y1, int x2, int y2);
void _th_rdimg(th_image *img, unsigned char *data);

#endif
