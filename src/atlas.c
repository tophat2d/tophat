#include "tophat.h"

extern th_global thg;

th_vf2 th_atlas_nth_coords(th_atlas *a, uu n) {
	return (th_vf2){.x = n % (int)a->dm.w, .y = (n - n%(int)a->dm.w) / (int)a->dm.h};
}

th_rect th_atlas_get_cell(th_atlas *a, th_vf2 cell) {
	th_image *i = th_get_image(a->i);
	return (th_rect) {
		.x = cell.x * a->cs.x / i->dm.x, .y = cell.y * a->cs.y / i->dm.y,
		.w = a->cs.x / i->dm.x, .h = a->cs.y / i->dm.y
	};
}
