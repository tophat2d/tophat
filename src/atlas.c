#include "tophat.h"

extern th_global *thg;

th_vf2
th_atlas_nth_coords(th_atlas *a, uu n)
{
	return (th_vf2){.x = n % (int)a->dm.w, .y = (n - n % (int)a->dm.w) / (int)a->dm.w};
}

th_rect
th_atlas_get_cell(th_atlas *a, th_vf2 cell)
{
	return (th_rect){.x = cell.x * a->cs.x / a->i->dm.x,
	    .y = cell.y * a->cs.y / a->i->dm.y,
	    .w = a->cs.x / a->i->dm.x,
	    .h = a->cs.y / a->i->dm.y};
}
