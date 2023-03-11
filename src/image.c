#include <math.h>
#include <string.h>
#include <stdint.h>

#include <assert.h>
#include <stb_image.h>

#include <sokol_gfx.h>

#include "tophat.h"
#ifdef __EMSCRIPTEN__
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else 
#include <GL/gl.h>
#endif

#ifndef GL_RGBA
#define GL_RGBA 0x1908
#endif
#ifndef GL_UNSIGNED_INT_8_8_8_8_REV
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#endif

extern th_global *thg;

void th_image_free(th_image *img) {
	sg_dealloc_image(img->tex);
}

static void th_image_free_umka(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image_free(p[0].ptrVal);
}

th_image *th_image_alloc() {
	return umkaAllocData(thg->umka, sizeof(th_image), th_image_free_umka);
}

uint32_t *th_image_get_data(th_image *img) {
	size_t size = sizeof(uint32_t) * img->dm.w * img->dm.h;
	uint32_t *data = malloc(size);
	
	GLuint tex = th_sg_get_gl_image(img->tex);

#ifdef __EMSCRIPTEN__
	glBindTexture(GL_TEXTURE_2D, tex);
	GLuint fbo;
	glGenFramebuffers(1, &fbo); 
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

	glReadPixels(0, 0, img->dm.w, img->dm.h, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
#else
	void glBindTexture(GLenum target, GLuint texture);
	GLenum glGetError();
	void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void *pixels);

	glBindTexture(GL_TEXTURE_2D, tex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data);
	glGetError();
#endif

	return data;
}

static void gen_tex(th_image *img, uint32_t *data) {
	img->tex = sg_make_image(&(sg_image_desc){
		.width = img->dm.w,
		.height = img->dm.h,
		.data.subimage[0][0] = (sg_range) {
			.ptr = data,
			.size = img->dm.w * img->dm.h * sizeof(uint32_t)
		},
		.pixel_format = SG_PIXELFORMAT_RGBA8,
		.mag_filter = img->filter,
		.min_filter = img->filter,
		.wrap_u = SG_WRAP_CLAMP_TO_EDGE,
		.wrap_w = SG_WRAP_CLAMP_TO_EDGE,
		.wrap_v = SG_WRAP_CLAMP_TO_EDGE,
		.usage = SG_USAGE_IMMUTABLE
	});

	sg_resource_state creation_status = sg_query_image_state(img->tex);
	if (creation_status != SG_RESOURCESTATE_VALID) {
		assert(creation_status != SG_RESOURCESTATE_INVALID && "gen_tex: INVALID");
		assert(creation_status != SG_RESOURCESTATE_FAILED && "gen_tex: FAILED");
		assert(false && "gen_tex: Unknown error");
	}
}

th_image *th_load_image(char *path) {
	int w, h, c;

	unsigned char *data = stbi_load(path, &w, &h, &c, STBI_rgb_alpha);
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

	gen_tex(img, (uint32_t *)data);

	stbi_image_free(data);

	return img;
}

void th_image_from_data(th_image *img, uint32_t *data, th_vf2 dm) {
	img->dm = dm;
	img->flipv = 0;
	img->fliph = 0;
	img->crop = (th_quad){
		(th_vf2){{0, 0}}, (th_vf2){{1, 0}},
		(th_vf2){{1, 1}}, (th_vf2){{0, 1}}};
	img->channels = 4;
	img->filter = SG_FILTER_NEAREST;

	gen_tex(img, data);
}

void th_image_set_filter(th_image *img, sg_filter filter) {
	uint32_t *data = th_image_get_data(img);
	img->filter = filter ? SG_FILTER_LINEAR : SG_FILTER_NEAREST;

	sg_dealloc_image(img->tex);
	gen_tex(img, data);

	free(data);
}

void th_image_update_data(th_image *img, uint32_t *data, th_vf2 dm) {
	sg_dealloc_image(img->tex);

	img->dm = dm;
	gen_tex(img, data);
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
	for (uu i=0; i < 4; i++) {
		q.v[i].x = trunc(q.v[i].x * thg->scaling + thg->offset.x);
		q.v[i].y = trunc(q.v[i].y * thg->scaling + thg->offset.y);
	}

	int sw, sh;
	th_window_get_dimensions(&sw, &sh);

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

	float verts[] = {
		q.tl.x / sw, q.tl.y / sh, bounds.tl.x, bounds.tl.y, 0, 0, 0, 0,
		q.tr.x / sw, q.tr.y / sh, bounds.tr.x, bounds.tr.y, 0, 0, 0, 0,
		q.br.x / sw, q.br.y / sh, bounds.br.x, bounds.br.y, 0, 0, 0, 0,
		q.tl.x / sw, q.tl.y / sh, bounds.tl.x, bounds.tl.y, 0, 0, 0, 0,
		q.br.x / sw, q.br.y / sh, bounds.br.x, bounds.br.y, 0, 0, 0, 0,
		q.bl.x / sw, q.bl.y / sh, bounds.bl.x, bounds.bl.y, 0, 0, 0, 0};

	for (uu i=0; i < 6; i++)
		memcpy(verts + (i * 8) + 4, colors, 4 * sizeof(float));

	th_canvas_batch_push_auto_flush(img, verts, sizeof(verts) / sizeof(verts[0]));
}

void th_image_set_as_render_target(th_image *img) {
	if (thg->has_render_target) {
		th_error("There already is a render target set!");
		return;
	}
	
	th_canvas_flush();
	sg_end_pass();
	sg_begin_pass(sg_make_pass(&(sg_pass_desc){
		.color_attachments[0].image = img->tex,
		.depth_stencil_attachment.image = img->tex,
		.label = "offscreen-pass"
	}), &thg->pass_action);
	
	thg->has_render_target = true;
	
	th_calculate_scaling(img->dm.w, img->dm.y);
}

void th_image_remove_render_target(th_image *img, th_vf2 wp) {
	if (!thg->has_render_target) {
		th_error("No render target is set.");
		return;
	}

	th_canvas_flush();
	sg_end_pass();
	
	sg_begin_default_pass(&thg->pass_action, sapp_width(), sapp_height());

	th_calculate_scaling(wp.x, wp.y);
	
	thg->has_render_target = false;
}

void th_image_init() {
	// TODO
}

void th_image_deinit() {
	return;
}
