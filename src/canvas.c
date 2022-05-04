#include "tophat.h"
#include <chew.h>
#include <GL/gl.h>
#include <string.h>
#include <math.h>
#include "pixelfont.h"
#include <stdio.h>

GLuint th_canvas_prog;

static GLuint vao;
static GLuint vbo;

#define BATCH 1024
static int cur_batch = 0;
static float batch_verts[BATCH * 3 * 6];

extern th_global thg;

void th_canvas_init() {
	const char *attribs[] = { "pos", "color" };
	th_canvas_prog = th_gl_create_prog(
		SHADER_BEGIN
		"attribute vec2 pos;\n"
		"attribute vec4 color;\n"
		"varying vec4 vcolor;\n"
		"void main() {\n"
		"  gl_Position = vec4(pos, 0, 1.0);\n"
		"  vcolor = color;\n"
		"}",

		SHADER_BEGIN
		"varying vec4 vcolor;\n"
		"void main() {\n"
		"  gl_FragColor = vcolor;\n"
		"}", attribs, 2);

	glGenVertexArrays(1, &vao);
	printf("%d\n", vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(batch_verts), batch_verts, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(2 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void th_canvas_flush() {
	if (!cur_batch) return;
	printf("flush\n");
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER,
		0, cur_batch * 3 * 6 * sizeof(float), batch_verts);

	glUseProgram(th_canvas_prog);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, cur_batch * 3);
	cur_batch = 0;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void th_canvas_triangle(uint32_t color, th_vf2 a, th_vf2 b, th_vf2 c) {
	float colors[4];
	for (int i=0; i < 4; ++i)
		colors[3 - i] = ((color >> (8 * i)) & 0xff) / (float)0xff;

	int sw, sh;
	th_window_get_dimensions(&sw, &sh);
	sh *= -1;
	sw /= 2;
	sh /= 2;

	const float verts[] = {
		(a.x + thg.offset.x) / sw - 1, (a.y + thg.offset.y) / sh + 1,
	 	(b.x + thg.offset.x) / sw - 1, (b.y + thg.offset.y) / sh + 1,
		(c.x + thg.offset.x) / sw - 1, (c.y + thg.offset.y) / sh + 1,
	};

	float *base = &batch_verts[cur_batch * 3 * 6];
	for (int i=0; i < 3; ++i) {
		memcpy(base, &verts[i * 2], sizeof(float) * 2);
		base += 2;
		memcpy(base, colors, sizeof(float) * 4);
		base += 4;
	}

	++cur_batch;
	if (cur_batch >= BATCH)
		th_canvas_flush();
}

void th_canvas_rect(uint32_t color, th_rect r) {
	r.x *= thg.scaling;
	r.y *= thg.scaling;
	r.w *= thg.scaling;
	r.h *= thg.scaling;
	th_canvas_triangle(color,
		(th_vf2){{r.x, r.y}},
		(th_vf2){{r.x + r.w, r.y}},
		(th_vf2){{r.x, r.y + r.h}});
	th_canvas_triangle(color,
		(th_vf2){{r.x + r.w, r.y}},
		(th_vf2){{r.x + r.w, r.y + r.h}},
		(th_vf2){{r.x, r.y + r.h}});
}

// stolen from here: stackoverflow.com/questions/1936934/turn-a-line-into-a-rectangle
void th_canvas_line(uint32_t color, th_vf2 f, th_vf2 t, fu thickness) {
	f.x *= thg.scaling;
	f.y *= thg.scaling;
	t.x *= thg.scaling;
	t.y *= thg.scaling;
	thickness *= thg.scaling;

	th_vf2 d = { .x = t.x - f.x, .y = t.y - f.y};
	fu mag = sqrt(d.x*d.x + d.y*d.y);
	d.x /= mag;
	d.y /= mag;
	th_vf2 ortho = { .x = -d.y * thickness * 0.5, .y = d.x * thickness * 0.5 };

	th_canvas_triangle(color,
		(th_vf2){{f.x - ortho.x, f.y - ortho.y}},
		(th_vf2){{f.x + ortho.x, f.y + ortho.y}},
		(th_vf2){{t.x + ortho.x, t.y + ortho.y}});
	th_canvas_triangle(color,
		(th_vf2){{f.x - ortho.x, f.y - ortho.y}},
		(th_vf2){{t.x + ortho.x, t.y + ortho.y}},
		(th_vf2){{t.x - ortho.x, t.y - ortho.y}});
}

void th_canvas_text(char *text, uint32_t color, th_vf2 p, fu size) {
	fu dx = p.x;

	for (;*text; text++) {
		char *glyph = pixelfont[(int)*text];
		if (*text == '\n') {
			p.x = dx;
			p.y += size * 6;
			continue;
		}

		for (int i=0; i < 25; ++i)
			if (glyph[i])
				th_canvas_rect(color,
					(th_rect){ p.x + (i%5)*size,
					p.y + (i/5)*size, size, size});
		p.x += size * 6;
	}
}
