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
	img->crop = (th_rect){0, 0, 1, 1};
	img->flipv = 0;
	img->fliph = 0;

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
	img->flipv = 0;
	img->fliph = 0;
	img->crop = (th_rect){0, 0, 1, 1};
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
void th_blit_tex(th_image *img, th_transform t) {
	CNFGFlushRender();

	th_quad q = th_ent_transform(
		&(th_ent){
			.rect = (th_rect){
				.w = (img->crop.w - img->crop.x) * img->dm.x,
				.h = (img->crop.h - img->crop.y) * img->dm.y},
			.t = t});

	for (uu i=0; i < 4; i++) {
		q.v[i].x *= thg.scaling;
		q.v[i].y *= thg.scaling;
	}

	th_vf2 p = th_quad_min(q);

	glUseProgram(gRDBlitProg);
	glUniform4f(gRDBlitProgUX,
		1.f/gRDLastResizeW, -1.f/gRDLastResizeH,
		-0.5f+p.x/(float)gRDLastResizeW, 0.5f-p.y/(float)gRDLastResizeH);
	glUniform1i(gRDBlitProgUT, 0);

	glBindTexture(GL_TEXTURE_2D, img->gltexture);

	const float verts[] = {
		q.tl.x - p.x, q.tl.y - p.y, q.tr.x - p.x, q.tr.y - p.y, q.br.x - p.x, q.br.y - p.y,
		q.tl.x - p.x, q.tl.y - p.y, q.br.x - p.x, q.br.y - p.y, q.bl.x - p.x, q.bl.y - p.y};

	th_rect bounds = img->crop;
	bounds.x *= 255;
	bounds.y *= 255;
	bounds.w *= 255;
	bounds.h *= 255;
	if (img->flipv)
		SWAP_VARS(bounds.y, bounds.h, fu);
	if (img->fliph)
		SWAP_VARS(bounds.x, bounds.w, fu);
	uint8_t tex_verts[] = {
		bounds.x, bounds.y, bounds.w, bounds.y, bounds.w, bounds.h,
		bounds.x, bounds.y, bounds.w, bounds.h, bounds.x, bounds.h };

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
	glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_TRUE, 0, tex_verts);

	glDrawArrays(GL_TRIANGLES, 0, 6);
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
