#include "tophat.h"
#include <chew.h>
#include <GL/gl.h>
#include <string.h>
#include <math.h>
#include "pixelfont.h"

extern th_global *thg;

int th_canvas_compile_shader(char *frag, char *vert) {
	const char *attribs[] = { "th_pos", "th_color" };
	return th_shader_compile(frag, vert,
		"attribute vec2 th_pos;\n"
		"attribute vec4 th_color;\n"
		"varying vec4 th_vcolor;\n"

		"vec2 th_vertex(vec2 vert);\n"

		"void main() {\n"
		"  gl_Position = vec4(th_vertex(th_pos), 0, 1.0);\n"
		"  th_vcolor = th_color;\n"
		"}\n",

		"varying vec4 th_vcolor;\n"

		"vec4 th_fragment(vec4 color);\n"

		"void main() {\n"
		"  gl_FragColor = th_fragment(th_vcolor);\n"
		"}\n", attribs, 2);
}

void th_canvas_init() {
	thg->canvas_prog = *th_get_shader_err(th_canvas_compile_shader(
		"vec2 th_vertex(vec2 vert) { return vert; }\n",
		"vec4 th_fragment(vec4 color) { return color; }\n"));

	glGenVertexArrays(1, &thg->canvas_vao);
	glGenBuffers(1, &thg->canvas_vbo);

	glBindVertexArray(thg->canvas_vao);

	glBindBuffer(GL_ARRAY_BUFFER, thg->canvas_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(thg->canvas_batch), thg->canvas_batch, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(2 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void th_canvas_flush() {
	if (!thg->canvas_batch_size) return;
	glBindBuffer(GL_ARRAY_BUFFER, thg->canvas_vbo);
	glBufferSubData(GL_ARRAY_BUFFER,
		0, thg->canvas_batch_size * 3 * 6 * sizeof(float), thg->canvas_batch);

	glUseProgram(thg->canvas_prog);
	glBindVertexArray(thg->canvas_vao);
	glDrawArrays(GL_TRIANGLES, 0, thg->canvas_batch_size * 3);
	thg->canvas_batch_size = 0;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void th_canvas_triangle(uint32_t color, th_vf2 a, th_vf2 b, th_vf2 c) {
	th_image_flush();

	float colors[4];
	for (int i=0; i < 4; ++i)
		colors[3 - i] = ((color >> (8 * i)) & 0xff) / (float)0xff;

	int sw, sh;
	th_gl_get_viewport_max(&sw, &sh);

	const float yoff = thg->has_framebuffer ? -1.0 : 1.0;
	const float verts[] = {
		(a.x + thg->offset.x) / sw - 1, (a.y + thg->offset.y) / sh + yoff,
	 	(b.x + thg->offset.x) / sw - 1, (b.y + thg->offset.y) / sh + yoff,
		(c.x + thg->offset.x) / sw - 1, (c.y + thg->offset.y) / sh + yoff,
	};

	float *base = &thg->canvas_batch[thg->canvas_batch_size * 3 * 6];
	for (int i=0; i < 3; ++i) {
		memcpy(base, &verts[i * 2], sizeof(float) * 2);
		base += 2;
		memcpy(base, colors, sizeof(float) * 4);
		base += 4;
	}

	++thg->canvas_batch_size;
	if (thg->canvas_batch_size >= BATCH_SIZE)
		th_canvas_flush();
}

void th_canvas_rect(uint32_t color, th_rect r) {
	r.x *= thg->scaling;
	r.y *= thg->scaling;
	r.w *= thg->scaling;
	r.h *= thg->scaling;
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
	f.x *= thg->scaling;
	f.y *= thg->scaling;
	t.x *= thg->scaling;
	t.y *= thg->scaling;
	thickness *= thg->scaling;

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
