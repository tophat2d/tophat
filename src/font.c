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

	return out;
}

void th_font_render_glyph(th_image *img, th_font *font,
                          uint32_t glyph, fu scale) {
	
	if (scale < 1) {
		th_error("Font size too small.\n");
		return;
	}

	scale = stbtt_ScaleForPixelHeight(font->info, 5 * scale);
	
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
