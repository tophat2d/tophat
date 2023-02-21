#include "tophat.h"
#include <string.h>
#include <math.h>
#include "pixelfont.h"

extern th_global *thg;

// pushes the vertices onto the batch
bool th_canvas_batch_push(float *array, size_t n) {
	if (n % BATCH_UNIT != 0) {
		// trying to push partial units (triangles), not allowed
		th_error("odd unit size");
		return false;
	}

	if ((thg->canvas_batch_size + n) > BATCH_LENGTH) {
		// not enough space
		return false;
	}

	memcpy(thg->canvas_batch+thg->canvas_batch_size, array, n * sizeof(float));
	thg->canvas_batch_size += n;

	return true;
}

// same as th_canvas_batch_push except automatically flushes the buffer if needed
int th_canvas_batch_push_auto_flush(float *array, size_t n) {
	bool ok = th_canvas_batch_push(array, n);
	if (!ok) { // if buffer is too small
		th_canvas_flush();
		ok = th_canvas_batch_push(array, n);
	}

	return ok;
}

void th_canvas_init() {
	thg->canvas_bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
		.data = SG_RANGE(thg->canvas_batch),
		.type = SG_BUFFERTYPE_VERTEXBUFFER,
		.usage = SG_USAGE_DYNAMIC,
		.label = "canvas-buffer"
	});
	
	sg_shader shd = sg_make_shader(&(sg_shader_desc){
		.vs.source =
			"#version 330\n"
			"layout(location=0) in vec2 pos;\n"
			"layout(location=1) in vec2 uv0;\n"
			"layout(location=2) in vec4 color0;\n"
			"out vec2 uv;\n"
			"out vec4 color;\n"
			"void main() {\n"
			"  gl_Position = pos;\n"
			"  uv = uv0;\n"
			"  color = color0;\n"
			"}\n",
		.fs.source =
			"#version 330\n"
			"in vec2 uv;\n"
			"in vec4 color;\n"
			"out vec4 frag_color;\n"
			"void main() {\n"
			"  frag_color = color + (uv * 0.00001);\n" // NOTE: I need to add this uv because otherwise it wont compile
			"}\n"																			 // 			it will be removed when texture support is added.
	});
	
	thg->canvas_pip = sg_make_pipeline(&(sg_pipeline_desc){
		.shader = shd,
		.layout = {
			.attrs = {
				[0].format = SG_VERTEXFORMAT_FLOAT2, // X, Y
				[1].format = SG_VERTEXFORMAT_FLOAT2, // U, V
				[2].format = SG_VERTEXFORMAT_FLOAT4  // R, G, B, A
			}
		},
		.label = "canvas-pip"
	});
}

static int batch_vertex_count() {
	if (thg->canvas_batch_size % BATCH_VERTEX != 0) {
		th_error("odd batch size")
	}
	return thg->canvas_batch_size/BATCH_VERTEX;
}

void th_canvas_flush() {
	// NOTE: (offset, no-vertices, no-instances)
  sg_draw(0, batch_vertex_count(), 1);
	thg->canvas_batch_size = 0;
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
		(a.x + thg->offset.x) / sw - 1, (a.y + thg->offset.y) / sh + yoff, 0, 0, 0, 0, 0, 0 // NOTE: Temporarily uvs are zeroed out
	 	(b.x + thg->offset.x) / sw - 1, (b.y + thg->offset.y) / sh + yoff, 0, 0, 0, 0, 0, 0
		(c.x + thg->offset.x) / sw - 1, (c.y + thg->offset.y) / sh + yoff, 0, 0, 0, 0, 0, 0
	};

	for (int i = 0; i < 3; ++i) {
		// +4 = color
		memcpy(verts+(i*BATCH_VERTEX+4), colors, sizeof(float) * 4);
	}

	th_canvas_batch_push_auto_flush(verts, sizeof verts);
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
