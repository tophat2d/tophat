
#include <stdint.h>

#include <GL/gl.h>

#ifdef __TINYC__
#define STBI_NO_SIMD
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#include "../lib/rawdraw/CNFG.h"
#include "../lib/rawdraw/chew.h"

#include "image.h"

#include "misc.h"

extern GLuint gRDShaderProg;
extern GLuint gRDBlitProg;
extern GLuint gRDShaderProgUX;
extern GLuint gRDBlitProgUX;
extern GLuint gRDBlitProgUT;
extern GLuint gRDBlitProgTex;
extern GLuint gRDLastResizeW;
extern GLuint gRDLastResizeH;

th_image *th_load_image(char *path) {
	int w, h, c;

	unsigned char *data = stbi_load(path, &w, &h, &c, 0);

	th_image *img;
	img = calloc(sizeof(th_image), 1);
	img->w = w;
	img->h = h;
	img->c = c;

	if (data == NULL) {
		th_error("could not find image at path %s", path);
		img->rdimg = NULL;
		return img;
	}

	// Faster way, but it doesn't seem to work. TODO FIXME
	/*if (c == 4) {
		img->rdimg = (unsigned int *)data;

		return img;
	}*/

	_th_rdimg(img, data);
	stbi_image_free(data);

	return img;
}

void th_free_image(th_image *img) {
	CNFGDeleteTex(img->tex);

	free(img->rdimg);
	free(img);
}

inline void th_image_from_data(th_image *img, uint32_t *data, int w, int h) {
	img->rdimg = malloc(sizeof(uint32_t) * w * h);
	memcpy(img->rdimg, data, sizeof(uint32_t) * w * h);
	img->w = w;
	img->h = h;
	img->c = 4;
}

void th_blit_tex(unsigned int tex, int x, int y, int w, int h, float rot) {
	if( w == 0 || h == 0 ) return;

	CNFGFlushRender();

	glUseProgram(gRDBlitProg);
	glUniform4f(gRDBlitProgUX,
		1.f/gRDLastResizeW, -1.f/gRDLastResizeH,
		-0.5f+x/(float)gRDLastResizeW, 0.5f-y/(float)gRDLastResizeH);
	glUniform1i(gRDBlitProgUT, 0);

	glBindTexture(GL_TEXTURE_2D, tex);

	float cx = w/2;
	float cy = h/2;

	float zrotx = 0;
	float zroty = 0;
	float brotx = w;
	float broty = h;
	float wrotx = w;
	float wroty = 0;
	float hrotx = 0;
	float hroty = h;

	if ( rot != 0 ) {
		th_rotate_point(&zrotx, &zroty, cx, cy, rot);
		th_rotate_point(&brotx, &broty, cx, cy, rot);
		th_rotate_point(&wrotx, &wroty, cx, cy, rot);
		th_rotate_point(&hrotx, &hroty, cx, cy, rot);
	}

	const float verts[] = {
		zrotx, zroty, wrotx, wroty, brotx, broty,
		zrotx, zroty, brotx, broty, hrotx, hroty };
	static const uint8_t colors[] = {
		0,0,   255,0,  255,255,
		0,0,  255,255, 0,255 };

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
	glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void th_image_flipv(th_image *img) {
	if (img->rdimg == NULL) {
		th_error("flipv: image is not valid");
		return;
	}

	uint32_t *f;
	f = malloc(sizeof(uint32_t) * img->w * img->h);

	for (int i=0; i < img->w; i++) for (int j=0; j < img->h; j++)
			f[(j + 1) * img->w - i - 1] = img->rdimg[j * img->w + i];

	free(img->rdimg);
	img->rdimg = f;
}

void th_image_fliph(th_image *img) {
	if (img->rdimg == NULL) {
		th_error("fliph: image is not valid");
		return;
	}

	uint32_t *f;
	f = malloc(sizeof(uint32_t) * img->w * img->h);
	for (int i=0; i < img->w; i++) for (int j=0; j < img->h; j++)
			f[(img->h - j - 1) * img->w + i] = img->rdimg[j * img->w + i];

	free(img->rdimg);
	img->rdimg = f;
}

void th_image_crop(th_image *img, int x1, int y1, int x2, int y2) {
	if (img->rdimg == NULL) {
		th_error("crop: image is not valid");
		return;
	}

	if (x1 < 0 || y1 < 0 || x2 >= img->w || y2 >= img->h) {
		th_error("crop: invalid dimensions");
		return;
	}

	if (x1 > x2) {
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}

	if (y1 > y2) {
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	uint32_t *n;
 	n = calloc(sizeof(uint32_t), (x2-x1) * (y2-y1));

	for (int x=0; x < x2-x1; x++) {
		for (int y=0; y < y2-y1; y++) {
			n[y*(x2-x1)+x] = img->rdimg[(y+y1)*img->w+x+x1];
		}
	}
	free(img->rdimg);
	img->w = x2-x1;
	img->h = y2-y1;
	img->rdimg = n;
}

void _th_rdimg(th_image *img, unsigned char *data) {
	uint32_t *rd;
	rd = malloc(sizeof(int) * img->w * img->h);
	uint32_t current = 0;

	for (int y=0; y < img->h; y += 1) {
		for (int x=0; x < img->w; x += 1) {
			current = 0;
			for (int i=0; i < img->c; i++) {
				current = current << 8;
				current += (uint32_t)data[(y * img->w + x) * img->c + i];
			}
			for (int i=0; i < 4 - img->c; i++) {
				if (current == 1) {
					current = 0x00 | current<<8;
					continue;
				}
				current = 0xff | current<<8;
			}
			rd[(y * img->w + x)] = current;
		}
	}
	img->rdimg = rd;
}
