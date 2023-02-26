#include <math.h>
#include <string.h>
#include <stdint.h>

#include "openglapi.h"
#include <stb_image.h>

#include "tophat.h"

#ifndef GL_UNSIGNED_INT_8_8_8_8 
#define GL_UNSIGNED_INT_8_8_8_8 0x8035
#endif

extern th_global *thg;

void th_image_free(th_image *img) {
	glDeleteTextures(1, &img->gltexture);
}

static void th_image_free_umka(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image_free(p[0].ptrVal);
}

th_image *th_image_alloc() {
	return umkaAllocData(thg->umka, sizeof(th_image), th_image_free_umka);
}

static
uint32_t *th_rdimg(th_image *img, unsigned char *data) {
	uint32_t *rd = malloc(sizeof(uint32_t) * img->dm.w * img->dm.h);

	for (int i=0; i < img->dm.w * img->dm.h; i++) {
		switch (img->channels) {
		case 3:
			rd[i] = 0;
			for (int j=0; j < 3; j++) {
				rd[i] |= data[i*3 + j] << j*8;
			}
			rd[i] |= 0xff << 3*8;
			break;
		case 4:
			rd[i] = 0;
			for (int j=0; j < 4; j++)
				rd[i] |= data[i*4 + j] << j*8;
			break;
		default:
			th_error("tophat can't load this image.");
			break;
		}
	}

	return rd;
}

uint32_t *th_image_get_data(th_image *img, bool rgba) {
	uint32_t *data = malloc(sizeof(uint32_t) * img->dm.w * img->dm.h);
	glBindTexture(GL_TEXTURE_2D, img->gltexture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	if (rgba)
		for (int i=0; i < img->dm.w * img->dm.h; ++i) {
			uint32_t c = data[i];
			data[i] = 
				(c >> 0*8 & 0xff) << 3*8 |
				(c >> 1*8 & 0xff) << 2*8 |
				(c >> 2*8 & 0xff) << 1*8 |
				(c >> 3*8 & 0xff) << 0*8;
		}
			

	return data;
}

th_image *th_load_image(char *path) {
	int w, h, c;

	unsigned char *data = stbi_load(path, &w, &h, &c, 0);
	if (!data) {
		th_error("Could not load an image at path %s.", path);
		return NULL;
	}

	th_image *img = th_image_alloc();
	img->dm.w = w;
	img->dm.h = h;
	img->channels = c;
	img->filter = SG_FILTER_NEAREST;
	img->crop = (th_quad){
		(th_vf2){{0, 0}}, (th_vf2){{1, 0}},
		(th_vf2){{1, 1}}, (th_vf2){{0, 1}}};
	img->flipv = 0;
	img->fliph = 0;

	img->tex = sg_make_image(&(sg_image_desc){
		.width = w,
		.height = h,
		.data.subimage = (sg_range) {
			.ptr = data,
			.size = w * h
		},
		.pixel_format = c == 4 ? SG_PIXELFORMAT_RGBA8UI : SG_PIXELFORMAT_RGB8UI,
		.mag_filter = img->filter,
		.min_filter = img->filter,
		.wrap_u = SG_WRAP_CLAMP_TO_EDGE,
		.wrap_w = SG_WRAP_CLAMP_TO_EDGE,
		.wrap_v = SG_WRAP_CLAMP_TO_EDGE
	});

	stbi_image_free(data);

	return img;
}

void th_image_from_data(th_image *img, uint32_t *data, th_vf2 dm) {
	for (int i=0; i < dm.w * dm.h; ++i) {
		uint32_t c = data[i];
		data[i] = 
			(c >> 0*8 & 0xff) << 3*8 |
			(c >> 1*8 & 0xff) << 2*8 |
			(c >> 2*8 & 0xff) << 1*8 |
			(c >> 3*8 & 0xff) << 0*8;
	}

	img->dm = dm;
	img->flipv = 0;
	img->fliph = 0;
	img->crop = (th_quad){
		(th_vf2){{0, 0}}, (th_vf2){{1, 0}},
		(th_vf2){{1, 1}}, (th_vf2){{0, 1}}};
	img->channels = 4;
	img->filter = 1;

	img->gltexture = th_gen_texture(data, dm, img->filter);
}

void th_image_set_filter(th_image *img, int filter) {
	uint32_t *data = th_image_get_data(img, false);

	img->filter = filter;
	glDeleteTextures(1, &img->gltexture);
	img->gltexture = th_gen_texture(data, img->dm, img->filter);

	free(data);
}

void th_image_update_data(th_image *img, uint32_t *data, th_vf2 dm) {
	for (int i=0; i < img->dm.w * img->dm.h; ++i) {
		uint32_t c = data[i];
		data[i] = 
			(c >> 0*8 & 0xff) << 3*8 |
			(c >> 1*8 & 0xff) << 2*8 |
			(c >> 2*8 & 0xff) << 1*8 |
			(c >> 3*8 & 0xff) << 0*8;
	}

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, img->gltexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dm.w, dm.h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, data);

	img->dm = dm;
}

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

void th_image_crop(th_image *img, th_vf2 tl, th_vf2 br) {
	img->crop = (th_quad){
		(th_vf2){{tl.x, tl.y}}, (th_vf2){{br.x, tl.y}},
		(th_vf2){{br.x, br.y}}, (th_vf2){{tl.x, br.y}}};
}

void th_image_render_transformed(th_image *img, th_transform trans, uint32_t color) {
	if (!img) return;

	th_quad q = img->crop;
	th_vf2 min = {{1e8, 1e8}};

	for (int i=0; i < 4; i++) {
		q.v[i].x *= img->dm.x;
		q.v[i].y *= img->dm.y;
		if (min.x > q.v[i].x) min.x = q.v[i].x;
		if (min.y > q.v[i].y) min.y = q.v[i].y;
	}

	trans.pos.x -= min.x*trans.scale.x;
	trans.pos.y -= min.y*trans.scale.y;
	trans.origin.x += min.x * trans.scale.x;
	trans.origin.y += min.y * trans.scale.y;
	th_transform_quad(&q, trans);
	th_blit_tex(img, q, color);
}

#define SWAP(a, b) { th_vf2 t = b; b = a; a = t; }

void th_blit_tex(th_image *img, th_quad q, uint32_t color) {
	th_canvas_flush();
	if (img->gltexture != thg->batch_tex || thg->blit_batch_size >= BATCH_SIZE - 1)
		th_image_flush();
	thg->batch_tex = img->gltexture;

	for (uu i=0; i < 4; i++) {
		q.v[i].x = trunc(q.v[i].x * thg->scaling + thg->offset.x);
		q.v[i].y = trunc(q.v[i].y * thg->scaling + thg->offset.y);
	}

	int sw, sh;
	th_gl_get_viewport_max(&sw, &sh);

	float colors[4];
		for (int i=0; i < 4; ++i)
			colors[3 - i] = ((color >> (8 * i)) & 0xff) / (float)0xff;

	th_quad bounds = img->crop;
	if (img->fliph) {
		SWAP(bounds.tl, bounds.bl);
		SWAP(bounds.tr, bounds.br);
	}

	if (img->flipv) {
		SWAP(bounds.tl, bounds.tr);
		SWAP(bounds.bl, bounds.br);
	}

	const float yoff = thg->has_framebuffer ? -1.0 : 1.0;
	const float verts[] = {
		q.tl.x / sw - 1.0, q.tl.y / sh + yoff, bounds.tl.x, bounds.tl.y,
		q.tr.x / sw - 1.0, q.tr.y / sh + yoff, bounds.tr.x, bounds.tr.y,
		q.br.x / sw - 1.0, q.br.y / sh + yoff, bounds.br.x, bounds.br.y,
		q.tl.x / sw - 1.0, q.tl.y / sh + yoff, bounds.tl.x, bounds.tl.y,
		q.br.x / sw - 1.0, q.br.y / sh + yoff, bounds.br.x, bounds.br.y,
		q.bl.x / sw - 1.0, q.bl.y / sh + yoff, bounds.bl.x, bounds.bl.y};

	float *ptr = &thg->blit_batch[thg->blit_batch_size * 6 * (2 + 2 + 4)];
	for (uu i=0; i < 6; i++) {
		memcpy(ptr, &verts[i * (2 + 2)], (2 + 2) * sizeof(float));
		ptr += 2 + 2;
		memcpy(ptr, colors, 4 * sizeof(float));
		ptr += 4;
	}

	++thg->blit_batch_size;
}

void th_image_set_as_render_target(th_image *img) {
	// TODO
}

void th_image_remove_render_target(th_image *img, th_vf2 wp) {
	// TODO
}

void th_image_init() {
	// TODO
}

void th_image_deinit() {
	return;
}
