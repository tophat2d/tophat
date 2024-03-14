#include "tophat.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

extern th_global *thg;

th_vf2
th_atlas_nth_coords(th_atlas *a, uu n)
{
	return (th_vf2){
	    .x = n % (int)a->dm.w,
	    .y = (n - n % (int)a->dm.w) / (int)a->dm.w,
	};
}

th_rect
th_atlas_get_cell(th_atlas *a, th_vf2 cell)
{
	return (th_rect){
	    .x = cell.x * a->cs.x / a->i->dm.x,
	    .y = cell.y * a->cs.y / a->i->dm.y,
	    .w = a->cs.x / a->i->dm.x,
	    .h = a->cs.y / a->i->dm.y,
	};
}

static void
blit(uint32_t *tgt, uint32_t tw, uint32_t th, uint32_t *src, uint32_t x0, uint32_t y0, uint32_t sw,
    uint32_t sh)
{
	for (uint32_t y = 0; y < sh; y++) {
		memcpy(tgt + (y0 + y) * tw + x0, src + y * sw, sw * sizeof(uint32_t));
	}
}

th_err
th_atlas_pack(th_atlas *a, void *arr, th_atlas_pack_strategy strategy)
{
	const size_t count = umkaGetDynArrayLen(arr);
	th_image **images = ((UmkaDynArray(th_image *) *)arr)->data;

	uint32_t w = 0, h = 0;
	for (int i = 0; i < count; i++) {
		w = MAX(w, images[i]->dm.w);
		h = MAX(h, images[i]->dm.h);
	}

	a->cs = (th_vf2){{w, h}};

	uint32_t iw, ih;
	uint32_t *data;

	switch (strategy) {
	case TH_ATLAS_PACK_SQUARE: {
		const uint32_t s = ceil(sqrt(count));
		a->dm = (th_vf2){{s, s}};
		iw = w * s;
		ih = h * s;
		data = calloc(sizeof(uint32_t), iw * ih);

		for (int i = 0; i < count; i++) {
			th_image *img = images[i];
			const uint32_t x = (i % s) * w + (w - img->dm.w) / 2;
			const uint32_t y = (i / s) * h + (h - img->dm.h) / 2;
			uint32_t *d = th_image_get_data(img);

			blit(data, iw, ih, d, x, y, img->dm.w, img->dm.h);
			free(d);
		}
		break;
	}
	case TH_ATLAS_PACK_ROW: {
		a->dm = (th_vf2){{count, 1}};
		iw = w * count;
		ih = h;
		data = calloc(sizeof(uint32_t), iw * ih);

		for (int i = 0; i < count; i++) {
			th_image *img = images[i];
			uint32_t *d = th_image_get_data(img);
			blit(data, iw, ih, d, i * w + (w - img->dm.w) / 2, (h - img->dm.h) / 2,
			    img->dm.w, img->dm.h);
			free(d);
		}

		break;
	}
	case TH_ATLAS_PACK_COLUMN: {
		a->dm = (th_vf2){{1, count}};
		iw = w;
		ih = h * count;
		data = calloc(sizeof(uint32_t), iw * ih);

		for (int i = 0; i < count; i++) {
			th_image *img = images[i];
			uint32_t *d = th_image_get_data(img);
			blit(data, iw, ih, d, (w - img->dm.w) / 2, i * h + (h - img->dm.h) / 2,
			    img->dm.w, img->dm.h);
			free(d);
		}

		break;
	}
	default: return th_err_bad_enum;
	}

	a->i = th_image_alloc();
	th_image_from_data(a->i, data, (th_vf2){{iw, ih}});

	return 0;
}
