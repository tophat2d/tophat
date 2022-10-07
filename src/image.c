#include <string.h>
#include <stdint.h>

#include <GL/gl.h>
#include <stb_image.h>
#include <chew.h>

#include "tophat.h"

#ifdef _WIN32
#define glActiveTexture glActiveTextureCHEW
#endif

extern th_global *thg;

void th_image_free(th_image *img) {
	glDeleteTextures(1, &img->gltexture);
	free(img->data);
}

static void th_image_free_umka(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image_free(p[0].ptrVal);
}

th_image *th_image_alloc() {
	return umkaAllocData(thg->umka, sizeof(th_image), th_image_free_umka);
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

	_th_rdimg(img, data);
	stbi_image_free(data);

	img->gltexture = th_gen_texture(img->data, img->dm, img->filter);

	return img;
}

void th_image_from_data(th_image *img, uint32_t *data, th_vf2 dm) {
	img->data = malloc(sizeof(uint32_t) * dm.w * dm.h);
	memcpy(img->data, data, sizeof(uint32_t) * dm.w * dm.h);
	img->dm = dm;
	img->flipv = 0;
	img->fliph = 0;
	img->crop = (th_quad){
		(th_vf2){{0, 0}}, (th_vf2){{1, 0}},
		(th_vf2){{1, 1}}, (th_vf2){{0, 1}}};
	img->channels = 4;
	img->filter = 1;

	img->gltexture = th_gen_texture(img->data, dm, img->filter);
}

void th_image_set_filter(th_image *img, int filter) {
	img->filter = filter;
	glDeleteTextures(1, &img->gltexture);
	img->gltexture = th_gen_texture(img->data, img->dm, img->filter);
}

void th_image_update_data(th_image *img, uint32_t *data, th_vf2 dm) {
	uint32_t *b = NULL;
	if ((b = realloc(img->data, sizeof(uint32_t) * dm.x * dm.y)) == NULL) {
		th_error("th_image_update_data: could not allocate\n");
		return;
	}

	img->data = b;
	memcpy(img->data, data, sizeof(uint32_t) * dm.x * dm.y);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, img->gltexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dm.w, dm.h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, img->data);

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
		colors[i] = ((color >> (8 * i)) & 0xff) / (float)0xff;

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

	glBindTexture(GL_TEXTURE_2D, img->gltexture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->dm.w, img->dm.h, 0, GL_RGBA,
		GL_UNSIGNED_INT_8_8_8_8, img->data);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int w,h;
	th_window_get_dimensions(&w, &h);
	glViewport(0, 0, w, h);
	thg->viewport.w = w;
	thg->viewport.h = h;
	thg->has_framebuffer = 0;
}

void _th_rdimg(th_image *img, unsigned char *data) {
	uint32_t *rd = malloc(sizeof(uint32_t) * img->dm.w * img->dm.h);

	for (int x=0; x < img->dm.w; x++) {
		for (int y=0; y < img->dm.h; y++) {
			int rd_index = (y * img->dm.w) + x;
			int data_index = ((y * img->dm.w) + x) * img->channels;
			rd[rd_index] = 0;

			for (int poff=0; poff < img->channels; poff++)
				rd[rd_index] += data[data_index + poff] << ((3 - poff) * 8);

			if (img->channels == 3)
				rd[rd_index] += 0xff;
		}
	}

	img->data = rd;
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

		"uniform sampler2D th_tex;\n"
		"varying vec2 th_tc;\n"
		"varying vec4 th_vcolor;\n"

		"vec4 th_fragment(sampler2D tex, vec2 coord);\n"

		"void main() { gl_FragColor = th_fragment(th_tex, th_tc) * th_vcolor.abgr;}\n",
		attribs, 3);
}

void th_image_init() {
	thg->blit_prog = *th_get_shader_err(th_image_compile_shader(
		"vec2 th_vertex(vec2 vert) { return vert; }",
		"vec4 th_fragment(sampler2D tex, vec2 coord) { return texture2D(tex, coord).abgr; }"));

	thg->blit_prog_tex = glGetUniformLocation(thg->blit_prog, "th_tex");

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
