#include "tophat.h"

void th_ray_getcoll(th_ray *ra, th_coll *colls, int maxColls,
                    int *collCount, th_ent **scene, int sceneLen) {
	// the other point of the ray
	th_vf2 p2 = {{ra->pos.x, ra->pos.y + ra->l}};
	th_rotate_point(&p2, ra->pos, ra->r);

	for (int i=0; i < sceneLen && *collCount < maxColls; i++) {
		th_quad q = th_ent_transform(scene[i]);

		if (th_line_to_quad(ra->pos, p2, &q, &colls[*collCount].pos) ||
		    // in case the ray is completely inside the quad
		    th_point_to_quad(ra->pos, &q, &colls[*collCount].pos)) {
			++(*collCount);
		}
	}
}
