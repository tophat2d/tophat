#include <string.h>
#include <stdint.h>

#include <GL/gl.h>
#include <stb_image.h>
#include <CNFG.h>
#include <chew.h>

#include "tophat.h"

extern GLuint gRDShaderProg;
extern GLuint gRDBlitProg;
extern GLuint gRDShaderProgUX;
extern GLuint gRDBlitProgUX;
extern GLuint gRDBlitProgUT;
extern GLuint gRDBlitProgTex;
extern GLuint gRDLastResizeW;
extern GLuint gRDLastResizeH;
#ifdef _WIN32
#define glActiveTexture glActiveTextureCHEW
#endif

extern th_global thg;

th_image *th_load_image(char *path) {
	int w, h, c;

	unsigned char *data = stbi_load(path, &w, &h, &c, 0);

	th_image *img;
	img = malloc(sizeof(th_image));
	img->dm.w = w;
	img->dm.h = h;
	img->channels = c;
	img->filter = 1;

	if (data == NULL) {
		th_error("could not find image at path %s", path);
		img->data = NULL;
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
	CNFGDeleteTex(img->gltexture);

	free(img->data);
	free(img);
}

void th_image_from_data(th_image *img, uint32_t *data, th_vf2 dm) {
	img->data = malloc(sizeof(uint32_t) * dm.w * dm.h);
	memcpy(img->data, data, sizeof(uint32_t) * dm.w * dm.h);
	img->dm = dm;
	img->channels = 4;
	img->filter = 1;

	img->gltexture = th_gen_texture(img->data, dm, img->filter);
}

void th_image_set_filter(th_image *img, int filter) {
	img->filter = filter;
	CNFGDeleteTex(img->gltexture);
	img->gltexture = th_gen_texture(img->data, img->dm, img->filter);
}

// stolen from rawdraw
unsigned int th_gen_texture(uint32_t *data, th_vf2 dm, unsigned filter) {
	GLuint tex;

	glGenTextures(1, &tex);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	if (filter)
		filter = GL_NEAREST;
	else
		filter = GL_LINEAR;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dm.w, dm.h, 0,  GL_RGBA,
		GL_UNSIGNED_BYTE, data);

	return (unsigned int)tex;
}

// stolen from rawdraw
void th_blit_tex(unsigned int tex, th_quad q) {
	CNFGFlushRender();

	th_vf2 p = th_quad_min(q);

	glUseProgram(gRDBlitProg);
	glUniform4f(gRDBlitProgUX,
		1.f/gRDLastResizeW, -1.f/gRDLastResizeH,
		-0.5f+p.x/(float)gRDLastResizeW, 0.5f-p.y/(float)gRDLastResizeH);
	glUniform1i(gRDBlitProgUT, 0);

	glBindTexture(GL_TEXTURE_2D, tex);

	const float verts[] = {
		q.tl.x - p.x, q.tl.y - p.y, q.tr.x - p.x, q.tr.y - p.y, q.br.x - p.x, q.br.y - p.y,
		q.tl.x - p.x, q.tl.y - p.y, q.br.x - p.x, q.br.y - p.y, q.bl.x - p.x, q.bl.y - p.y};
	static const uint8_t colors[] = {
		0,0,   255,0,  255,255,
		0,0,  255,255, 0,255 };

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
	glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

// TODO do this inplace
void th_image_flipv(th_image *img) {
	if (img->data == NULL) {
		th_error("flipv: image is not valid");
		return;
	}

	uint32_t *f = malloc(sizeof(uint32_t) * img->dm.w * img->dm.h);

	for (int i=0; i < img->dm.w; i++) for (int j=0; j < img->dm.h; j++)
			f[(j + 1) * (uu)img->dm.w - i - 1] = img->data[j * (uu)img->dm.w + i];

	free(img->data);
	img->data = f;
	CNFGDeleteTex(img->gltexture);
	img->gltexture = th_gen_texture(img->data, img->dm, img->filter);
}

void th_image_fliph(th_image *img) {
	if (img->data == NULL) {
		th_error("fliph: image is not valid");
		return;
	}

	uint32_t *f = malloc(sizeof(uint32_t) * img->dm.w * img->dm.h);
	for (int i=0; i < img->dm.w; i++) for (int j=0; j < img->dm.h; j++)
			f[(uu)((img->dm.h - j - 1) * img->dm.w + i)] = img->data[(uu)(j * img->dm.w + i)];

	free(img->data);
	img->data = f;
	CNFGDeleteTex(img->gltexture);
	img->gltexture = th_gen_texture(img->data, img->dm, img->filter);
}

void th_image_crop(th_image *img, th_vf2 tl, th_vf2 br) {
	if (img->data == NULL) {
		th_error("crop: image is not valid");
		return;
	}

	if (tl.x < 0 || tl.y < 0 || br.x > img->dm.w || br.y > img->dm.h) {
		th_error("crop: invalid dimensions");
		return;
	}

	if (tl.x > br.x) {
		int tmp = tl.x;
		tl.x = br.x;
		br.x = tmp;
	}

	if (tl.y > br.y) {
		int tmp = tl.y;
		tl.y = br.y;
		br.y = tmp;
	}

 	uint32_t *n = calloc(sizeof(uint32_t), (br.x-tl.x) * (br.y-tl.y));

	for (int x=0; x < br.x-tl.x; x++) {
		for (int y=0; y < br.y-tl.y; y++) {
			n[(uu)(y*(br.x-tl.x)+x)] = img->data[(uu)((y+tl.y)*img->dm.w+x+tl.x)];
		}
	}
	free(img->data);
	img->dm.w = br.x-tl.x;
	img->dm.h = br.y-tl.y;
	img->data = n;
	CNFGDeleteTex(img->gltexture);
	img->gltexture = th_gen_texture(img->data, img->dm, img->filter);
}

void _th_rdimg(th_image *img, unsigned char *data) {
	uint32_t *rd = malloc(sizeof(uint32_t) * img->dm.w * img->dm.h);

	for (int x=0; x < img->dm.w; x++) {
		for (int y=0; y < img->dm.h; y++) {
			int rd_index = (y * img->dm.w) + x;
			int data_index = ((y * img->dm.w) + x) * img->channels;
			rd[rd_index] = 0;

			for (int poff=0; poff < img->channels; poff++) {
				rd[rd_index] += data[data_index + poff] << ((3 - poff) * 8);
			}

			if (img->channels == 3)
				rd[rd_index] += 0xff;
		}
	}

	img->data = rd;
}

void th_image_deinit() {
	while (thg.image_count--) {
		th_image *img = thg.images[thg.image_count];
		CNFGDeleteTex(img->gltexture);
		free(img->data);
		free(img);
	}
}
