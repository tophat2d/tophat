#include "tophat.h"

int th_ray_getcoll(th_ray *ra, th_ent **scene, int count, th_vf2 *ic) {
	th_vf2 p = {{ra->pos.x, ra->pos.y + ra->l}};
	th_rotate_point(&p, ra->pos, ra->r);

	for (int i=0; i < count; i++) {
		th_quad q = th_ent_transform(scene[i]);

		if (th_line_to_quad(ra->pos, p, &q, ic))
			return i;

		if (th_point_to_quad(ra->pos, &q, ic))
			return i;
	}
	return 0;
}
