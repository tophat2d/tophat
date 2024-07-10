#include "tophat.h"
#include "umka_api.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PACK_OVERSAMPLING 1
#define PACK_MAXRES 2048
#define PACK_CHARSPERPAGE TH_FONTPLATFORM_CHARSPERPAGE
#define PACK_PAGECOUNT TH_FONTPLATFORM_PAGECOUNT

extern th_global *thg;

// NOTE(skejeton): Initializes out_page->pc
static bool
f_pack_attempt(th_font *font, uint32_t char_offset, th_font_atlas_page *out_page,
    uint8_t *out_buffer, uint32_t out_buffer_size)
{
	stbtt_pack_context pack_context = {0};

	if (stbtt_PackBegin(&pack_context, out_buffer, out_buffer_size, out_buffer_size,
		out_buffer_size, 1, NULL) == false) {
		th_error("Failed to begin pack!");
		return false;
	}

	stbtt_PackSetOversampling(&pack_context, PACK_OVERSAMPLING, PACK_OVERSAMPLING);
	if (stbtt_PackFontRange(&pack_context, font->data, 0, font->size, char_offset,
		PACK_CHARSPERPAGE, out_page->pc) == false) {
		stbtt_PackEnd(&pack_context);
		// probably bitmap size is too smol
		return false;
	}

	stbtt_PackEnd(&pack_context);

	return true;
}

static bool
f_pack_atlas_page(th_font *font, uint32_t char_offset, th_font_atlas_page *out_page)
{
	bool packed = false;

	for (int size = 1; !packed && size < PACK_MAXRES; size *= 2) {
		uint8_t *buffer = malloc(size * size);

		if (f_pack_attempt(font, char_offset, out_page, buffer, size)) {
			uint32_t *rgba = malloc(size * size * sizeof(*rgba));
			for (int i = 0; i < size * size; ++i) {
				rgba[i] = 0x00FFFFFFU | ((uint32_t)buffer[i] << (uint32_t)24);
			}

			th_image_from_data(&out_page->img, rgba, (th_vf2){{size, size}});
			th_image_set_filter(&out_page->img, font->filter);

			packed = true;
			free(rgba);
		}

		free(buffer);
	}

	return packed;
}

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

static th_font_atlas_page *
f_get_page(th_font *font, uint32_t page_index)
{
	if (page_index >= PACK_PAGECOUNT) {
		th_error("Attempt to access font page index at or over " STRINGIFY(
		    PACK_PAGECOUNT) ". Oh dear.");
	}

	th_font_atlas_page **pages = font->pages;

	if (pages[page_index] == NULL) {
		pages[page_index] = calloc(sizeof *pages[0], 1);
		if (f_pack_atlas_page(font, page_index * PACK_CHARSPERPAGE, pages[page_index]) ==
		    false) {
			th_error("I failed allocating page %d for the font atlas, make sure you "
				 "don't put the font size too big.",
			    page_index);

			free(pages[page_index]);
			pages[page_index] = NULL;
		}
	}

	return pages[page_index];
}

static th_rect
f_get_dest_rect(th_font *font, stbtt_aligned_quad quad)
{
	return (th_rect){
	    quad.x0, quad.y0 + font->ascent * font->scale, quad.x1 - quad.x0, quad.y1 - quad.y0};
}

typedef struct
{
	const char *string;
	th_font *font;
	uint32_t prev_glyph;
	float x, y, xMax, yMax, initX, initY, scale;
} f_iterator;

typedef struct
{
	th_quad quad;
	th_image *img;
} f_glyph;

static f_iterator
f_iterator_begin(th_font *font, const char *string, float x, float y, float scale)
{
	return (f_iterator){.string = string, .font = font, .initX = x, .initY = y, .scale = scale};
}

static bool
f_iterator_going(f_iterator *iter)
{
	// FIXME(skejeton): Hack eh?
	while (*iter->string == '\n') {
		iter->y += (iter->font->ascent - iter->font->descent) * iter->font->scale;
		iter->x = 0;
		iter->string += 1;
	}

	return *iter->string;
}

static stbtt_aligned_quad
f_iterator_next_quad(f_iterator *iter, th_font_atlas_page **out_page)
{
	uint32_t glyph;
	iter->string += th_utf8_decode(&glyph, iter->string);
	th_font_atlas_page *page = f_get_page(iter->font, glyph / PACK_CHARSPERPAGE);

	stbtt_aligned_quad quad;

	iter->x += stbtt_GetCodepointKernAdvance(&iter->font->info, iter->prev_glyph, glyph) *
	    iter->font->scale;
	stbtt_GetPackedQuad(page->pc, page->img.dm.x, page->img.dm.y,
	    glyph - (glyph / PACK_CHARSPERPAGE) * PACK_CHARSPERPAGE, &iter->x, &iter->y, &quad, 1);

	// NOTE(skejeton): The letters will look crooked if i don't do this, especially applies to
	// something like "|||||"
	iter->x = floor(iter->x);

	// FIXME(skejeton): hack?
	if (out_page) {
		*out_page = page;
	}

	iter->prev_glyph = glyph;
	return quad;
}

static void
f_iterator_cap_measurements(f_iterator *iter)
{
	if (iter->x > iter->xMax) {
		iter->xMax = iter->x;
	}

	iter->yMax = iter->y + (-iter->font->descent + iter->font->ascent) * iter->font->scale;
}

static f_glyph
f_iterator_next_glyph(f_iterator *iter)
{
	th_font_atlas_page *page;
	stbtt_aligned_quad quad = f_iterator_next_quad(iter, &page);

	// the quad image data is actually uvs hooray!
	page->img.crop = (th_quad){(th_vf2){{quad.s0, quad.t0}}, (th_vf2){{quad.s1, quad.t0}},
	    (th_vf2){{quad.s1, quad.t1}}, (th_vf2){{quad.s0, quad.t1}}};
	th_rect rect = f_get_dest_rect(iter->font, quad);

	// NOTE(skejeton): This is to scale font, it's bad but it works ish
	rect.x *= iter->scale;
	rect.y *= iter->scale;
	rect.w *= iter->scale;
	rect.h *= iter->scale;

	rect.x += iter->initX;
	rect.y += iter->initY;

	th_quad q = {0};
	q.v[0] = (th_vf2){{rect.x, rect.y}};
	q.v[1] = (th_vf2){{rect.x + rect.w, rect.y}};
	q.v[2] =
	    (th_vf2){{rect.x + rect.w, rect.y + rect.h}}; // Third value in quad is bottom right..
	q.v[3] = (th_vf2){{rect.x, rect.y + rect.h}};

	return (f_glyph){
	    .quad = q,
	    .img = &page->img,
	};
}

static void *
read_font_data(char *path)
{
	FILE *f = fopen(path, "rb");
	if (f == NULL) {
		th_error("Can't access font at %s. (Check if it exists)", path);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char *buf = malloc(size);
	if (buf == NULL) {
		th_error("Can't allocate buffer for font.");
		return NULL;
	}

	size = fread(buf, size, 1, f);
	fclose(f);

	return buf;
}

static void
free_font(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_font *font = p[0].ptrVal;
	for (int i = 0; i < PACK_PAGECOUNT; ++i) {
		if (font->pages[i]) {
			th_image_free(&font->pages[i]->img);
			free(font->pages[i]);
		}
	}

	free(font->data);
}

th_font *
th_font_alloc()
{
	return umkaAllocData(thg->umka, sizeof(th_font), free_font);
}

th_err
th_font_load(th_font **out, char *path, double size, uint32_t filter)
{
	*out = NULL;

	char regularized_path[4096];
	th_regularize_path(path, "./", regularized_path, sizeof regularized_path);
	path = regularized_path;

	void *data = read_font_data(path);
	if (data == NULL) {
		return th_err_io;
	}

	th_font *font = th_font_alloc();
	if (font == NULL) {
		free(data);
		return th_err_alloc;
	}

	{
		font->filter = filter; // filter
		font->size = size;     // size
		font->data = data;

		stbtt_InitFont(&font->info, font->data, 0); // info

		float scale = stbtt_ScaleForPixelHeight(&font->info, font->size);
		int ascent, descent;
		stbtt_GetFontVMetrics(&font->info, &ascent, &descent, NULL);

		font->scale = scale;	 // scale
		font->ascent = ascent;	 // ascent
		font->descent = descent; // descent
	}

	*out = font;
	return 0;
}

th_vf2
th_font_measure(th_font *font, const char *s)
{
	// What else??
	if (s == NULL) {
		return (th_vf2){{0, 0}};
	}

	if (font == NULL) {
		th_error("Font is null!");
		return (th_vf2){0};
	}

	f_iterator iter = f_iterator_begin(font, s, 0, 0, 1.0);

	while (f_iterator_going(&iter)) {
		f_iterator_next_quad(&iter, NULL);
		f_iterator_cap_measurements(&iter);
	}

	// Account for trailing newlines offseting the Y coordiante, if the text ends with newlines.
	f_iterator_cap_measurements(&iter);

	return (th_vf2){{iter.xMax, iter.yMax}};
}

void
th_font_draw(th_font *font, const char *s, double x, double y, uint32_t color, double scale)
{
	// What else??
	if (s == NULL) {
		return;
	}

	if (font == NULL) {
		return;
	}

	f_iterator iter = f_iterator_begin(font, s, x, y, scale);

	while (f_iterator_going(&iter)) {
		f_glyph glyph = f_iterator_next_glyph(&iter);

		th_image_draw_quad(glyph.img, glyph.quad, color);
	}

	return;
}

void
th_font_deinit()
{
	// NOTE(skejeton): I don't think I need to free anything here, should be automatically
	// freed.
}
