#include "tophat.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stb_truetype.h>

extern th_global thg;

th_font *th_font_load(char *path) {
	FILE *f = fopen(path, "rb");
	if (f == NULL) {
		th_error("Could not find font at %s.", path);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char *buf = malloc(size);
	if (!buf) {
		th_error("Could not allocate buffer of size %d for font data.", size);
		return NULL;
	}
	fread(buf, size, 1, f);
	fclose(f);

	th_font *out = th_alloc_font();
	if (!out) {
		th_error("Could not allocate font");
		free(buf);
	}

	out->buf = buf;
	out->info = malloc(sizeof(stbtt_fontinfo));
	if (!out->info) {
		th_error("Could not allocate font info.");
		return NULL;
	}
	stbtt_InitFont(out->info, buf, 0);

	out->scale = stbtt_ScaleForPixelHeight(out->info, 5);

	return out;
}

void th_font_render_glyph(th_image *img, th_font *font,
                          uint32_t glyph, fu scale) {
	
	if (scale < 1) {
		th_error("Font size too small.\n");
		return;
	}

	scale *= font->scale;
	
	int x0, y0, x1, y1;
	stbtt_GetFontBoundingBox(font->info, &x0, &y0, &x1, &y1);
	x0 *= scale;
	y0 *= scale;
	x1 *= scale;
	y1 *= scale;

	int ascent;
	stbtt_GetFontVMetrics(font->info, &ascent, 0, 0);
	int baseline = (int)(ascent * scale);

	int w, h;
	uint8_t *bmp = stbtt_GetCodepointBitmap(font->info, scale, scale, glyph, &w, &h, 0, 0);

	int offset;
	stbtt_GetCodepointBitmapBox(font->info, glyph, scale, scale, 0, &offset, 0, 0);
	offset = baseline + offset;
	if (offset < 0) {
		offset = 0;
	}


	uint32_t *imgdata = calloc(sizeof(uint32_t), w * (offset + h));
	for (int x=0; x < w; ++x)
		for (int y=0; y < h; ++y)
			imgdata[(y + offset)*w + x] = 0xffffff00 | bmp[y*w + x];
	free(bmp);

	th_image_from_data(img, imgdata, (th_vf2){.w=w, .h=offset + h});
	free(imgdata);
}

void th_font_deinit() {
	while (thg.font_count--) {
		free(thg.fonts[thg.font_count].info);
		free(thg.fonts[thg.font_count].buf);
	}
	free(thg.fonts);
}

// cached font

static uint32_t font_cache_hash(uint32_t r) {
	uint32_t hash = 5381;
	while (r) {
		hash = (((hash << 5) + hash) + (r & 0xff)) & 0xffffffff;
		r = r >> 8;
	}
	return hash;
}

static th_font_cache_item *font_cache_get(th_font_cache *fc, uint32_t r) {
	uint32_t hash = font_cache_hash(r) % fc->len;
	if (fc->data[hash].r != r) return NULL;
	return &fc->data[hash];
}

void th_cached_font_draw(th_cached_font *c, char *str,
                         th_vf2 pos, uint32_t color, fu scale) {

	scale /= c->size;
	fu lx = pos.x;
	th_font *font = th_get_font_err(c->font);
	
	uint32_t lg = 0, r;
	do {
		str += th_utf8_decode(&r, str);
		const th_font_cache_item *fci = font_cache_get(&c->cache, r);
		if (!fci) {
			th_error("You forgot to cache '%c' (U+%d). Use font.Cached.preCacheStr to cache it.\n", r, r);
			break;
		}

		const uint32_t g = fci->g;

		if (lg)
			pos.x += c->size * scale *
				stbtt_GetGlyphKernAdvance(font->info, lg, g)*font->scale;

		int advance;
		stbtt_GetGlyphHMetrics(font->info, g, &advance, 0);
		if (r == ' ') {
			pos.x += c->size * scale * advance*font->scale;
			continue;
		}

		if (r == '\n') {
			pos.y += 5 * 1.1 * scale;
			pos.x = lx;
			continue;
		}

		th_image *i = th_get_image(fci->i);
		th_blit_tex(i, (th_quad){{
				.tl = (th_vf2){ .x = pos.x                , .y = pos.y                 },
				.tr = (th_vf2){ .x = pos.x + scale*i->dm.w, .y = pos.y                 },
				.br = (th_vf2){ .x = pos.x + scale*i->dm.w, .y = pos.y + scale*i->dm.h },
				.bl = (th_vf2){ .x = pos.x                , .y = pos.y + scale*i->dm.h },
			}}, color);

		pos.x += c->size * scale * advance*font->scale;
		lg = g;
	} while (*str);
}
