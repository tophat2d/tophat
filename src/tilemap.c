
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "tophat.h"

extern th_global *thg;

void
th_tmap_draw(th_tmap *t, th_transform tr)
{
	th_image *a = t->a.i;
	if (!a)
		return;

	int tw = t->w, th = umkaGetDynArrayLen(&t->cells) / t->w;

	for (int i = 0; i < tw; i++)
		for (int j = 0; j < th; j++) {
			if (t->cells.data[j * t->w + i] == 0)
				continue;
			int cell = t->cells.data[j * t->w + i];

			const float w = t->a.cs.x * t->scale, h = t->a.cs.y * t->scale,
				    x = t->pos.x + w * i, y = t->pos.y + h * j;

			th_transform tt = {0};
			tt.pos = (th_vf2){{x, y}};
			tt.scale = (th_vf2){{t->scale, t->scale}};
			th_vf2 p = th_atlas_nth_coords(&t->a, cell - 1);
			th_rect r = th_atlas_get_cell(&t->a, p);
			a->crop = (th_quad){(th_vf2){{r.x, r.y}}, (th_vf2){{r.x + r.w, r.y}},
			    (th_vf2){{r.x + r.w, r.y + r.h}}, (th_vf2){{r.x, r.y + r.h}}};

			th_quad q = {0};
			th_transform_rect(&q, tt, (th_rect){.w = t->a.cs.x, .h = t->a.cs.y});
			th_transform_quad(&q, tr);
			th_image_draw_quad(a, q, 0xffffffff);
		}
}

static uu
safe_get(uu *src, uu w, uu h, uu x, uu y)
{
	if (x > w || y > h)
		return 0;

	return src[w * y + x];
}

enum
{
	top = 1,
	right = 2,
	bot = 4,
	left = 8
};

void
th_tmap_autotile(uu *tgt, uu *src, uu w, uu h, uu *tiles, uu limiter)
{
	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++) {
			if (src[x + y * w] != limiter)
				continue;
			int sum = 0;
			sum += top * !!safe_get(src, w, h, x, y - 1);
			sum += bot * !!safe_get(src, w, h, x, y + 1);
			sum += right * !!safe_get(src, w, h, x + 1, y);
			sum += left * !!safe_get(src, w, h, x - 1, y);
			tgt[x + y * w] = tiles[sum];
		}
}
