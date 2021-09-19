#include "tophat.h"
#include <stdlib.h>
#include <stdio.h>
#include <stb_truetype.h>

struct {
	uint32_t rune;
	uint8_t *data;
	int w, h;
	int yoff;
} typedef __th_font_char;

void th_font_load(th_font *out, char *path) {
	FILE *f = fopen(path, "r");
	if (f == NULL) {
		th_error("Could not find font at %s.", path);
		out = NULL;
		return;
	}

	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char *buf = malloc(size);
	fread(buf, size, 1, f);
	fclose(f);

	out->info = malloc(sizeof(stbtt_fontinfo));
	stbtt_InitFont(out->info, buf, 0);
}

void th_str_to_img(
	th_image *out, th_font *font,
	uint32_t *runes, int runec,
	double scale, uint32_t color,
	int ax, int ay) {
	__th_font_char *chars = malloc(runec * sizeof(__th_font_char));

	int
		cw = 0, rh = 0,
		w = 0, h = 0;
	for (int i=0; i < runec; i++) {
		chars[i].rune = runes[i];

		int trash;
		// get bitmap
		chars[i].data = stbtt_GetCodepointBitmap(
			font->info, scale, scale, runes[i],
			&chars[i].w, &chars[i].h, &trash, &trash);

		// get offset (for characters like q)
		stbtt_GetCodepointBitmapBox(
			font->info, runes[i], scale, scale,
			&trash, &chars[i].yoff, &trash, &trash);

		if (i < runec-1)
			// add kerning
			chars[i].w += stbtt_GetCodepointKernAdvance(font->info, runes[i], runes[i+1]) * scale;

		if (chars[i].rune == '\n') {
			if (cw > w)
				w = cw;

			h += rh;
			cw = 0;
			continue;
		}

		if (chars[i].rune == ' ')
			w += w/i;

		if (chars[i].h + ay > rh)
			rh = chars[i].h + ay;

		w += chars[i].w + ax;
	}
	// letters like q might not have enough space. kind of a hack
	h += 1.5 * rh;

	uint32_t *data = malloc(sizeof(uint32_t) * w * h);
	int
		x=0, y=0;
	for (int i=0; i < runec; i++) {
		if (chars[i].rune == '\n') {
			x = 0;
			y += rh;
			continue;
		}

		if (chars[i].rune == ' ')
			x += x/i; // this is a weird way to do spaces. TODO

		for (int py=y; py - y < chars[i].h; py++)
			for (int px=x; px - x < chars[i].w; px++)
				data[(py + (rh - chars[i].h) + (chars[i].h + chars[i].yoff)) * w + px] =
					color | chars[i].data[(py - y) * chars[i].w + (px - x)];
		free(chars[i].data);

		x += chars[i].w + ax;
	}
	free(chars);

	th_image_from_data(out, data, w, h);
	free(data);
}
