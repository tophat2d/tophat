#include "tophat.h"
#include <umka_api.h>

extern th_global *thg;

th_vf2 th_atlas_nth_coords(th_atlas *a, uu n) {
	return (th_vf2){
		.x = n % (int)a->dm.w,
		.y = (n - n%(int)a->dm.w) / (int)a->dm.w
	};
}

th_rect th_atlas_get_cell(th_atlas *a, th_vf2 cell) {
	return (th_rect) {
		.x = cell.x * a->cs.x / a->i->dm.x, .y = cell.y * a->cs.y / a->i->dm.y,
		.w = a->cs.x / a->i->dm.x, .h = a->cs.y / a->i->dm.y
	};
}

void th_atlas_draw_matrix(th_atlas *atl, void/*[]iu*/ *_m, uu w, th_transform *t) {
	const UmkaDynArray(iu) *m = _m;
	const uu h = umkaGetDynArrayLen(m) / w;

	for (int y=0; y < h; ++y) {
		for (int x=0; x < w; ++x) {
			const iu tile = m->data[x + y*w];
			if (tile < 0 || tile > atl->dm.w * atl->dm.h)
				continue;

			th_transform tr = *t;
			th_vf2 p = { .x = x * atl->cs.x, .y = y * atl->cs.y };
			th_rotate_point(&p, t->origin, t->rot);
			tr.pos.x += p.x * tr.scale.x;
			tr.pos.y += p.y * tr.scale.y;
			tr.origin.x = atl->cs.x / 2;
			tr.origin.y = atl->cs.y / 2;

			const th_rect r = th_atlas_get_cell(atl, th_atlas_nth_coords(atl, tile));
			th_image_crop(atl->i,
				(th_vf2){ .x = r.x, .y = r.y },
				(th_vf2){ .x = r.x + r.w, .y = r.y + r.h }
			);

			th_image_render_transformed(atl->i, tr, 0xffffffff);
		}
	}
}
