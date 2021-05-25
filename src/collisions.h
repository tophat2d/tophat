#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "polygon.h"
#include "tilemap.h"
#include <stdbool.h>

int _th_poly_to_poly(th_poly *p1, th_poly *p2, int *ix, int *iy);
int _th_poly_to_line(th_poly *a, int sx, int sy, int ex, int ey, int *ix, int *iy);
int _th_line_to_line(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int *ix, int *iy);
int _th_poly_to_point(th_poly *a, int px, int py, int *ix, int *iy);
bool _th_coll_on_tilemap(th_poly *p, th_tmap *t, int *rx, int *ry);

#endif
