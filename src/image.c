#include <string.h>
#include <stdint.h>

#include <GL/gl.h>
#include <stb_image.h>
#include <chew.h>

#include "tophat.h"

// declaration needed to leech off of rawdraw
#ifdef _WIN32
#define glActiveTexture glActiveTextureCHEW
#endif

GLuint th_blit_prog = -1;
GLuint th_blit_prog_color = -1;
GLuint th_blit_prog_pr = -1;
GLuint th_blit_prog_pos = -1;
GLuint th_blit_prog_tex = -1;
static GLuint vao;
static GLuint vbo;

extern th_global thg;

th_image *th_load_image(char *path) {
	int w, h, c;

	unsigned char *data = stbi_load(path, &w, &h, &c, 0);
	if (!data) {
		th_error("Could not load an image at path %s.", path);
		return NULL;
	}

	th_image *img = th_alloc_image();
	img->dm.w = w;
	img->dm.h = h;
	img->channels = c;
	img->filter = 1;
	img->crop = (th_rect){0, 0, 1, 1};
	img->flipv = 0;
	img->fliph = 0;

	_th_rdimg(img, data);
	stbi_image_free(data);

	return img;
}

void th_free_image(th_image *img) {
	glDeleteTextures(1, &img->gltexture);

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
	glDeleteTextures(1, &img->gltexture);
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
void th_blit_tex(th_image *img, th_transform t, uint32_t color) {
	th_canvas_flush();

	th_quad q = {0};
	th_transform_rect(&q, t, (th_rect){
				.w = (img->crop.w - img->crop.x) * img->dm.x,
				.h = (img->crop.h - img->crop.y) * img->dm.y});

	for (uu i=0; i < 4; i++) {
		q.v[i].x *= thg.scaling;
		q.v[i].x += thg.offset.x;
		q.v[i].y *= thg.scaling;
		q.v[i].y += thg.offset.y;
	}

	int sw, sh;
	th_window_get_dimensions(&sw, &sh);
	sh *= -0.5;
	sw *= 0.5;

	glUseProgram(th_blit_prog);
	glUniform1i(th_blit_prog_tex, 0);

	float colors[4];
	for (int i=0; i < 4; ++i)
		colors[i] = ((color >> (8 * i)) & 0xff) / (float)0xff;
	glUniform4f(th_blit_prog_color, colors[3], colors[2], colors[1], colors[0]);

	glBindTexture(GL_TEXTURE_2D, img->gltexture);

	th_rect bounds = img->crop;
	if (img->flipv)
		SWAP_VARS(bounds.y, bounds.h, fu);
	if (img->fliph)
		SWAP_VARS(bounds.x, bounds.w, fu);
	const float verts[] = {
		q.tl.x / sw - 1.0, q.tl.y / sh + 1.0, bounds.x, bounds.y,
		q.tr.x / sw - 1.0, q.tr.y / sh + 1.0, bounds.w, bounds.y,
		q.br.x / sw - 1.0, q.br.y / sh + 1.0, bounds.w, bounds.h,
		q.tl.x / sw - 1.0, q.tl.y / sh + 1.0, bounds.x, bounds.y,
		q.br.x / sw - 1.0, q.br.y / sh + 1.0, bounds.w, bounds.h,
		q.bl.x / sw - 1.0, q.bl.y / sh + 1.0, bounds.x, bounds.h};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 4 * 6, verts);

	glUseProgram(th_blit_prog);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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

void th_image_init() {
	const char *attribs[] = {
		"vert", "tex_vert"
	};
	th_blit_prog = th_gl_create_prog(
		SHADER_BEGIN
		"attribute vec2 vert;\n"
		"attribute vec2 tex_vert;\n"
		"varying vec2 tc;\n"
		"void main() {\n"
		"  gl_Position = vec4( vert.xy, 0, 1 );\n"
		"  tc = tex_vert;\n"
		"}\n",
		
		SHADER_BEGIN
		"precision mediump float;\n"
		"uniform sampler2D tex;\n"
		"uniform vec4 color;\n"
		"varying vec2 tc;\n"
		"void main() { gl_FragColor = texture2D(tex,tc).abgr * color;}",
		attribs, 2);

	th_blit_prog_color = glGetUniformLocation(th_blit_prog, "color");
	th_blit_prog_tex = glGetUniformLocation(th_blit_prog, "tex");

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	const float verts[6 * 4];
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, verts, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void th_image_deinit() {
	while (thg.image_count--) {
		th_image img = thg.images[thg.image_count];
		glDeleteTextures(1, &img.gltexture);
		free(img.data);
	}
	free(thg.images);
}
