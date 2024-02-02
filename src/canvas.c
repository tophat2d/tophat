#include "pixelfont.h"
#include "tophat.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef EMSCRIPTEN
#include "shader-web.glsl.h"
#else
#include "shader.glsl.h"
#endif

extern th_global *thg;

struct
{
	sg_buffer *buffers;
	size_t buffers_count;
	size_t buffers_cap;
	size_t current_buffer;
	size_t starting_buffer; // buffer to start flush at
} typedef buffer_cache_t;

buffer_cache_t buffer_cache;

static sg_buffer
alloc_buffer()
{
	// 1: check if we already have a cached buffer
	if (buffer_cache.current_buffer < buffer_cache.buffers_count) {
		return buffer_cache.buffers[buffer_cache.current_buffer++];
	}

	// 2: check if we need to resize the buffers array
	assert(buffer_cache.buffers_count <= buffer_cache.buffers_cap && "flaw: count > cap");
	if (buffer_cache.buffers_cap == buffer_cache.buffers_count) {
		if (buffer_cache.buffers_cap == 0) {
			buffer_cache.buffers_cap = 1;
		} else {
			buffer_cache.buffers_cap *= 2;
		}
		buffer_cache.buffers =
		    realloc(buffer_cache.buffers, buffer_cache.buffers_cap * sizeof(sg_buffer));
	}

	// 3: alloc a new buffer
	sg_buffer new_buffer = sg_make_buffer(&(sg_buffer_desc){.size = sizeof(thg->canvas_batch),
	    .type = SG_BUFFERTYPE_VERTEXBUFFER,
	    .usage = SG_USAGE_STREAM});

	buffer_cache.current_buffer++;
	buffer_cache.buffers[buffer_cache.buffers_count++] = new_buffer;
	return new_buffer;
}

static void
buffer_cache_reset()
{
	buffer_cache.current_buffer = 0;
	buffer_cache.starting_buffer = 0;
}

enum
{
	SCISSOR_NONE,
	SCISSOR_START,
	SCISSOR_END
};

struct
{
	int scissor_stage;
	th_rect scissor_rect;
	size_t start, end;
	th_image *img;
} typedef phase;

phase *phases;
size_t phases_len;
size_t phases_cap;

static void
finalize_last_phase()
{
	if (phases_len > 0) {
		phases[phases_len - 1].end = thg->canvas_batch_size;
	}
}

static phase *
alloc_phase()
{
	assert(phases_len <= phases_cap && "flaw: len > cap");
	if (phases_len == phases_cap) {
		if (phases_cap == 0) {
			phases_cap = 1;
		} else {
			phases_cap *= 2;
		}
		phases = realloc(phases, phases_cap * sizeof(phase));
	}

	return &phases[phases_len++];
}

static void
push_phase(phase phs)
{
	size_t start = 0;
	if (phases_len > 0) {
		if (phases[phases_len - 1].scissor_stage == SCISSOR_NONE) {
			phases[phases_len - 1].end = thg->canvas_batch_size;
			if (phs.scissor_stage == SCISSOR_NONE &&
			    phases[phases_len - 1].img == phs.img) {
				return;
			}
		}
		start = phases[phases_len - 1].end;
	}

	phs.start = start;
	phs.end = thg->canvas_batch_size;
	phase *p = alloc_phase();
	if (p) {
		*p = phs;
	}
}

void
th_canvas_begin_scissor_rect(th_rect rect)
{
	// prevent invalid value in sokol
	if (rect.w < 0)
		rect.w = 0;
	if (rect.h < 0)
		rect.h = 0;

	rect.x *= thg->scaling;
	rect.y *= thg->scaling;
	rect.w *= thg->scaling;
	rect.h *= thg->scaling;

	rect.x += thg->offset.x - thg->wp_offset.x;
	rect.y += thg->offset.y - thg->wp_offset.y;

	push_phase((phase){.scissor_stage = SCISSOR_START, .scissor_rect = rect});
}

void
th_canvas_end_scissor()
{
	push_phase((phase){.scissor_stage = SCISSOR_END});
}

static void
push_image_phase(th_image *img)
{
	push_phase((phase){.img = img});
}

// pushes the vertices onto the batch
bool
th_canvas_batch_push(float *array, size_t n)
{
	assert(n % BATCH_UNIT == 0 && "odd units");

	if ((thg->canvas_batch_size + n) >= BATCH_LENGTH) {
		// not enough space
		return false;
	}

	memcpy(thg->canvas_batch + thg->canvas_batch_size, array, n * sizeof(float));
	thg->canvas_batch_size += n;

	return true;
}

void
th_canvas_use_image(th_image *img)
{
}

// same as th_canvas_batch_push except automatically flushes the buffer if needed
void
th_canvas_batch_push_auto_flush(th_image *img, float *array, size_t n)
{
	push_image_phase(img);
	thg->canvas_image = img;

	if (!th_canvas_batch_push(array, n)) {
		// if buffer is too small
		th_canvas_flush();

		push_image_phase(img);
		thg->canvas_image = img;
		th_canvas_batch_push(array, n);
	}
}

static th_image white_img;

void
th_canvas_init()
{
	thg->canvas_bind = (sg_bindings){0};

	const sg_shader_desc *shd_desc = th_shader_desc(sg_query_backend());
	if (shd_desc == NULL) {
		th_error("shader is null!");
	}
	sg_shader shd = sg_make_shader(shd_desc);
	thg->main_shader = shd;
	thg->canvas_pip = sg_make_pipeline(&(sg_pipeline_desc){.shader = shd,
	    .layout = {.attrs =
			   {
			       [0].format = SG_VERTEXFORMAT_FLOAT2, // X, Y
			       [1].format = SG_VERTEXFORMAT_FLOAT2, // U, V
			       [2].format = SG_VERTEXFORMAT_FLOAT4  // R, G, B, A
			   }},
	    .colors[0].blend =
		{
		    .enabled = true,
		    .src_factor_alpha = SG_BLENDFACTOR_ONE,
		    .dst_factor_alpha = SG_BLENDFACTOR_ZERO,
		    .op_alpha = SG_BLENDOP_ADD,
		    .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
		    .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		    .op_rgb = SG_BLENDOP_ADD,
		},
	    .label = "canvas-pip"});

	uint32_t white = 0xffffffff;
	th_image_from_data(&white_img, &white, (th_vf2){.w = 1, .h = 1});

	thg->canvas_bind.fs.images[SLOT_tex] = white_img.tex;
	thg->canvas_image = &white_img;
	thg->canvas_bind.fs.samplers[SLOT_smp] = white_img.smp;
}

void
th_canvas_deinit()
{
	th_image_free(&white_img);
	sg_destroy_pipeline(thg->canvas_pip);
	sg_destroy_shader(thg->main_shader);
}

void
th_canvas_end_frame()
{
	buffer_cache_reset();
}

void
th_canvas_flush()
{
	sg_buffer buf = alloc_buffer();
	sg_update_buffer(buf, &SG_RANGE(thg->canvas_batch));
	thg->canvas_bind.vertex_buffers[0] = buf;
	finalize_last_phase();
	for (size_t i = 0; i < phases_len; i++) {
		phase *phs = &phases[i];
		switch (phs->scissor_stage) {
		case SCISSOR_NONE:
			thg->canvas_bind.fs.images[SLOT_tex] = phs->img->tex;
			thg->canvas_bind.fs.samplers[SLOT_smp] = phs->img->smp;
			sg_apply_bindings(&thg->canvas_bind);
			sg_draw(
			    phs->start / BATCH_VERTEX, (phs->end - phs->start) / BATCH_VERTEX, 1);
			break;
		case SCISSOR_START:
			sg_apply_scissor_rectf(phs->scissor_rect.x, phs->scissor_rect.y,
			    phs->scissor_rect.w, phs->scissor_rect.h, true);
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

void
th_canvas_triangle(uint32_t color, th_vf2 a, th_vf2 b, th_vf2 c)
{
	a.x -= thg->wp_offset.x;
	a.y -= thg->wp_offset.y;
	b.x -= thg->wp_offset.x;
	b.y -= thg->wp_offset.y;
	c.x -= thg->wp_offset.x;
	c.y -= thg->wp_offset.y;
	a.x *= thg->scaling;
	a.y *= thg->scaling;
	b.x *= thg->scaling;
	b.y *= thg->scaling;
	c.x *= thg->scaling;
	c.y *= thg->scaling;

	float colors[4];
	for (int i = 0; i < 4; ++i)
		colors[3 - i] = ((color >> (8 * i)) & 0xff) / (float)0xff;

	float sw = thg->target_size.x, sh = thg->target_size.y;

	// clang-format off
	float verts[] = {
		(a.x + thg->offset.x) / sw, (a.y + thg->offset.y) / sh, 0, 0, 0, 0, 0, 0, // NOTE: Temporarily uvs are zeroed out
		(b.x + thg->offset.x) / sw, (b.y + thg->offset.y) / sh, 0, 0, 0, 0, 0, 0,
		(c.x + thg->offset.x) / sw, (c.y + thg->offset.y) / sh, 0, 0, 0, 0, 0, 0
	};
	// clang-format on

	for (int i = 0; i < 3; ++i) {
		// +4 = color
		memcpy(verts + (i * BATCH_VERTEX + 4), colors, sizeof(float) * 4);
	}

	th_canvas_batch_push_auto_flush(&white_img, verts, sizeof verts / sizeof verts[0]);
}

void
th_canvas_rect(uint32_t color, th_rect r)
{
	th_canvas_triangle(
	    color, (th_vf2){{r.x, r.y}}, (th_vf2){{r.x + r.w, r.y}}, (th_vf2){{r.x, r.y + r.h}});
	th_canvas_triangle(color, (th_vf2){{r.x + r.w, r.y}}, (th_vf2){{r.x + r.w, r.y + r.h}},
	    (th_vf2){{r.x, r.y + r.h}});
}

// stolen from here: stackoverflow.com/questions/1936934/turn-a-line-into-a-rectangle
void
th_canvas_line(uint32_t color, th_vf2 f, th_vf2 t, fu thickness)
{
	th_vf2 d = {.x = t.x - f.x, .y = t.y - f.y};
	fu mag = sqrt(d.x * d.x + d.y * d.y);
	d.x /= mag;
	d.y /= mag;
	th_vf2 ortho = {.x = -d.y * thickness * 0.5, .y = d.x * thickness * 0.5};

	th_canvas_triangle(color, (th_vf2){{f.x - ortho.x, f.y - ortho.y}},
	    (th_vf2){{f.x + ortho.x, f.y + ortho.y}}, (th_vf2){{t.x + ortho.x, t.y + ortho.y}});
	th_canvas_triangle(color, (th_vf2){{f.x - ortho.x, f.y - ortho.y}},
	    (th_vf2){{t.x + ortho.x, t.y + ortho.y}}, (th_vf2){{t.x - ortho.x, t.y - ortho.y}});
}

void
th_canvas_text(char *text, uint32_t color, th_vf2 p, fu size)
{
	if (text == NULL)
		return;

	fu dx = p.x;

	for (; *text; text++) {
		char *glyph = pixelfont[(int)*text];
		if (*text == '\n') {
			p.x = dx;
			p.y += size * 6;
			continue;
		}

		for (int i = 0; i < 25; ++i)
			if (glyph[i])
				th_canvas_rect(color,
				    (th_rect){
					p.x + (i % 5) * size, p.y + (i / 5) * size, size, size});
		p.x += size * 6;
	}
}

void
th_canvas_quad(th_quad *q, uint32_t color)
{
	th_canvas_triangle(color, q->tl, q->tr, q->br);
	th_canvas_triangle(color, q->tl, q->br, q->bl);
}
