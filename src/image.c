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
	img->filter = 1;
	img->crop = (th_quad){
		(th_vf2){{0, 0}}, (th_vf2){{1, 0}},
		(th_vf2){{1, 1}}, (th_vf2){{0, 1}}};
	img->flipv = 0;
	img->fliph = 0;

	uint32_t *udata = th_rdimg(img, data);
	stbi_image_free(data);

	img->gltexture = th_gen_texture(udata, img->dm, img->filter);
	free(udata);

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

#define SWAP(a, b) { th_vf2 t = a; b = a; a = t; }

void th_blit_tex(th_image *img, th_quad q, uint32_t color) {
	th_canvas_flush();
	if (img->gltexture != thg->batch_tex || thg->blit_batch_size >= BATCH_SIZE - 1)
		th_image_flush();
	thg->batch_tex = img->gltexture;

	for (uu i=0; i < 4; i++) {
		q.v[i].x *= thg->scaling;
		q.v[i].x += thg->offset.x;
		q.v[i].y *= thg->scaling;
		q.v[i].y += thg->offset.y;
	}

	int sw, sh;
	th_gl_get_viewport_max(&sw, &sh);

	float colors[4];
		for (int i=0; i < 4; ++i)
			colors[3 - i] = ((color >> (8 * i)) & 0xff) / (float)0xff;

	th_quad bounds = img->crop;
	if (img->flipv) {
		SWAP(bounds.tl, bounds.bl);
		SWAP(bounds.tr, bounds.br);
	}

	if (img->fliph) {
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

void th_image_flush() {
	if (!thg->blit_batch_size) return;

	glBindBuffer(GL_ARRAY_BUFFER, thg->blit_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, thg->blit_batch_size * 6 * (4 + 2 + 2) * sizeof(float), thg->blit_batch);

	glUniform1i(thg->blit_prog_tex, 0);
	glBindTexture(GL_TEXTURE_2D, thg->batch_tex);

	glUseProgram(thg->blit_prog);
	glBindVertexArray(thg->blit_vao);
	glDrawArrays(GL_TRIANGLES, 0, thg->blit_batch_size * 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	thg->blit_batch_size = 0;
}

void th_image_set_as_render_target(th_image *img) {
	if (thg->has_framebuffer) {
		th_error("Another image is alredy selected as the render target.");
		return;
	}
		
	if (img->dm.w >= RENDER_TARGET_MAX_SIZE || img->dm.h >= RENDER_TARGET_MAX_SIZE) {
		th_error("Render target can be atmost %dx%d big.",
			RENDER_TARGET_MAX_SIZE, RENDER_TARGET_MAX_SIZE);
		return;
	}

	th_image_flush();
	th_canvas_flush();

	thg->scaling = 1;
	thg->offset.x = 0;
	thg->offset.y = 0;
	thg->viewport = img->dm;
	thg->has_framebuffer = 1;

	glBindFramebuffer(GL_FRAMEBUFFER, thg->framebuffer);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, img->gltexture, 0);

	GLenum drawBuffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &drawBuffer);

	glViewport(0, 0, img->dm.w, img->dm.h);

	glUseProgram(thg->blit_prog);
	glUniform1i(thg->framebuffer_uniform, 1);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		th_error("Could not setup image render target %x, %x.", glGetError(), glCheckFramebufferStatus(GL_FRAMEBUFFER));
}

void th_image_remove_render_target(th_image *img, th_rect *cam) {
	if (!thg->has_framebuffer) {
		th_error("No render target is set.");
		return;
	}

	th_canvas_flush();
	th_image_flush();

	th_calculate_scaling(cam->w, cam->h);

	/*glBindTexture(GL_TEXTURE_2D, img->gltexture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->dm.w, img->dm.h, 0, GL_RGBA,
		GL_UNSIGNED_INT_8_8_8_8, img->data);*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(thg->blit_prog);
	glUniform1i(thg->framebuffer_uniform, 0);

	int w,h;
	th_window_get_dimensions(&w, &h);
	glViewport(0, 0, w, h);
	thg->viewport.w = w;
	thg->viewport.h = h;
	thg->has_framebuffer = 0;
}

int th_image_compile_shader(char *frag, char *vert) {
	const char *attribs[] = {
		"th_vert", "th_tex_vert", "th_color"
	};

	return th_shader_compile(frag, vert,
		"attribute vec2 th_vert;\n"
		"attribute vec2 th_tex_vert;\n"
		"attribute vec4 th_color;\n"
		"varying vec2 th_tc;\n"
		"varying vec4 th_vcolor;\n"

		"vec2 th_vertex(vec2 vert);\n"

		"void main() {\n"
		"  th_vcolor = th_color;\n"
		"  gl_Position = vec4( th_vertex(th_vert), 0, 1 );\n"
		"  th_tc = th_tex_vert;\n"
		"}\n",

		"uniform int th_has_framebuffer;\n"
		"uniform sampler2D th_tex;\n"
		"varying vec2 th_tc;\n"
		"varying vec4 th_vcolor;\n"

		"vec4 th_fragment(sampler2D tex, vec2 coord);\n"

		"void main() {\n"
		"  gl_FragColor = th_fragment(th_tex, th_tc) * th_vcolor; \n"
		"}\n",
		attribs, 3);
}

void th_image_init() {
	thg->blit_prog = *th_get_shader_err(th_image_compile_shader(
		"vec2 th_vertex(vec2 vert) { return vert; }",
		"vec4 th_fragment(sampler2D tex, vec2 coord) { return texture2D(tex, coord); }"));

	thg->blit_prog_tex = glGetUniformLocation(thg->blit_prog, "th_tex");
	thg->framebuffer_uniform = glGetUniformLocation(thg->blit_prog, "th_has_framebuffer");

	glGenVertexArrays(1, &thg->blit_vao);
	glGenBuffers(1, &thg->blit_vbo);

	glBindVertexArray(thg->blit_vao);

	glBindBuffer(GL_ARRAY_BUFFER, thg->blit_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(thg->blit_batch), thg->blit_batch, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(2 * sizeof(float)));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(4 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenFramebuffers(1, &thg->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, thg->framebuffer);
		glGenRenderbuffers(1, &thg->depthbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, thg->depthbuffer);
			glRenderbufferStorage(
				GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
				RENDER_TARGET_MAX_SIZE, RENDER_TARGET_MAX_SIZE
			);

			glFramebufferRenderbuffer(
				GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_RENDERBUFFER, thg->depthbuffer
			);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void th_image_deinit() { }
