#include "tophat.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "shader.glsl.h"
#include "pixelfont.h"

extern th_global *thg;

enum {
	SCISSOR_NONE,
	SCISSOR_START,
	SCISSOR_END
};

struct {
	int scissor_stage;
	th_rect scissor_rect;
	size_t start, end;
	th_image *img;
}
typedef phase;

phase phases[256];
size_t phases_len;

static void finalize_last_phase() {
	if (phases_len > 0) {
		phases[phases_len-1].end = thg->canvas_batch_size;
	}
}

static phase *alloc_phase() {
	if (phases_len >= 256) {
		th_error("phase overflow");
		return NULL;
	}

	return &phases[phases_len++];
}

static void push_phase(phase phs) {
	size_t start = 0;
	if (phases_len > 0) {
		if (phases[phases_len-1].scissor_stage == SCISSOR_NONE) {
			phases[phases_len-1].end = thg->canvas_batch_size;
			if (phs.scissor_stage == SCISSOR_NONE && phases[phases_len-1].img == phs.img) {
				return;
			}
		}
		start = phases[phases_len-1].end;
	}

	phs.start = start;
	phs.end = thg->canvas_batch_size;
	phase *p = alloc_phase();
	if (p) {
		*p = phs;
	}
}

void th_canvas_begin_scissor_rect(th_rect rect) {
	// prevent invalid value in sokol
	if (rect.w < 0) rect.w = 0;
	if (rect.h < 0) rect.h = 0;

	push_phase((phase){
		.scissor_stage = SCISSOR_START,
		.scissor_rect = rect
	});
}

void th_canvas_end_scissor() {
	push_phase((phase){
		.scissor_stage = SCISSOR_END
	});
}

static void push_image_phase(th_image *img) {
	push_phase((phase){
		.img = img
	});
}

// pushes the vertices onto the batch
bool th_canvas_batch_push(float *array, size_t n) {
	if (n % BATCH_UNIT != 0) {
		// trying to push partial units (triangles), not allowed
		th_error("odd unit size");
		return false;
	}

	if ((thg->canvas_batch_size + n) >= BATCH_LENGTH) {
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
		th_error("buffer too small");
		return 0;
	}

	return ok;
}

static th_image white_img;

void th_canvas_init() {
	thg->canvas_bind = (sg_bindings){0};
	thg->canvas_bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
		.size = sizeof(thg->canvas_batch),
		.type = SG_BUFFERTYPE_VERTEXBUFFER,
		.usage = SG_USAGE_STREAM,
		.label = "canvas-buffer"
	});

	sg_shader shd = sg_make_shader(th_shader_desc(sg_query_backend()));
	thg->canvas_pip = sg_make_pipeline(&(sg_pipeline_desc){
		.shader = shd,
		.layout = {
			.attrs = {
				[0].format = SG_VERTEXFORMAT_FLOAT2, // X, Y
				[1].format = SG_VERTEXFORMAT_FLOAT2, // U, V
				[2].format = SG_VERTEXFORMAT_FLOAT4  // R, G, B, A
			}
		},
		.colors[0].blend = {
			.enabled = true,
			.src_factor_alpha = SG_BLENDFACTOR_ONE,
			.dst_factor_alpha = SG_BLENDFACTOR_ZERO,
			.op_alpha = SG_BLENDOP_ADD,
			.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
			.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
			.op_rgb = SG_BLENDOP_ADD,
		},
		.label = "canvas-pip"
	});

	uint32_t white = 0xffffffff;
	th_image_from_data(&white_img, &white, (th_vf2){ .w = 1, .h = 1 });

	thg->canvas_bind.fs_images[SLOT_tex] = white_img.tex;
	thg->canvas_image = &white_img;
}

void th_canvas_deinit() {
	th_image_free(&white_img);
}

void th_canvas_flush() {
	finalize_last_phase();
	sg_update_buffer(thg->canvas_bind.vertex_buffers[0], &SG_RANGE(thg->canvas_batch));
	for (size_t i = 0; i < phases_len; i++) {
		phase *phs = &phases[i];
		switch (phs->scissor_stage) {
		case SCISSOR_NONE:
			thg->canvas_bind.fs_images[SLOT_tex] = phs->img->tex;
			sg_apply_bindings(&thg->canvas_bind);
			sg_draw(phs->start/BATCH_VERTEX, (phs->end - phs->start)/BATCH_VERTEX, 1);
			break;
		case SCISSOR_START:
			sg_apply_scissor_rectf(phs->scissor_rect.x, phs->scissor_rect.y, phs->scissor_rect.w, phs->scissor_rect.h, true);
			break;
		case SCISSOR_END:
			sg_apply_scissor_rectf(0, 0, sapp_widthf(), sapp_heightf(), true);
			break;
		}
	}
	phases_len = 0;
	// NOTE: (offset, no-vertices, no-instances)
	thg->canvas_batch_size = 0;
}

void th_canvas_use_image(th_image *img) {
	push_image_phase(img);
	thg->canvas_image = img;
}

void th_canvas_triangle(uint32_t color, th_vf2 a, th_vf2 b, th_vf2 c) {
	th_canvas_use_image(&white_img);

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
	th_window_get_dimensions(&sw, &sh);

	float verts[] = {
		(a.x + thg->offset.x) / sw, (a.y + thg->offset.y) / sh, 0, 0, 0, 0, 0, 0, // NOTE: Temporarily uvs are zeroed out
	 	(b.x + thg->offset.x) / sw, (b.y + thg->offset.y) / sh, 0, 0, 0, 0, 0, 0,
		(c.x + thg->offset.x) / sw, (c.y + thg->offset.y) / sh, 0, 0, 0, 0, 0, 0
	};

	for (int i = 0; i < 3; ++i) {
		// +4 = color
		memcpy(verts+(i*BATCH_VERTEX+4), colors, sizeof(float) * 4);
	}

	th_canvas_batch_push_auto_flush(verts, sizeof verts / sizeof verts[0]);
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
