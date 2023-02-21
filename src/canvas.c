#include "tophat.h"
#include <string.h>
#include <math.h>
#include "pixelfont.h"

extern th_global *thg;

#define BATCH_SIZE 1024

void th_canvas_init() {
	thg->canvas_bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
		.size = BATCH_SIZE * 3 * (2 + 4) * sizeof(float),
		.type = SG_BUFFERTYPE_VERTEXBUFFER,
		.usage = SG_USAGE_DYNAMIC,
		.label = "canvas-buffer"
	});
	
	sg_shader shd = sg_make_shader(&(sg_shader_desc){
		.vs.source =
			"#version 330\n"
			"layout(location=0) in vec2 pos;\n"
			"layout(location=1) in vec4 color0;\n"
			"out vec4 color\n"
			"void main() {\n"
			"  gl_Position = pos;\n"
			"  color = color0;\n"
			"}\n",
		.fs.source =
			"#version 330\n"
			"in vec4 color;\n"
			"out vec4 frag_color;\n"
			"void main() {\n"
			"  frag_color = color;\n"
			"}\n"
	});
	
	thg->canvas_pip = sg_make_pipeline(&(sg_pipeline_desc){
		.shader = shd,
		.layout = {
			.attrs = {
				[0].format = SG_VERTEXFORMAT_FLOAT2,
				[1].format = SG_VERTEXFORMAT_FLOAT4
			}
		},
		.label = "canvas-pip"
	});
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

	a.x *= thg->scaling;
	a.y *= thg->scaling;
	b.x *= thg->scaling;
	b.y *= thg->scaling;
	c.x *= thg->scaling;
	c.y *= thg->scaling;

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

void th_canvas_quad(th_quad *q, uint32_t color) {
	th_canvas_triangle(color, q->tl, q->tr, q->br);
	th_canvas_triangle(color, q->tl, q->br, q->bl);
}
