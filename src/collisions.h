#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "tilemap.h"

int collbyentity(entnode_t *a, entity *e);
int polytopoly(poly *p1, poly *p2, int *ix, int *iy);
int polytoline(poly *a, int sx, int sy, int ex, int ey, int *ix, int *iy);
int linetoline(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int *ix, int *iy);
int polytopoint(poly *a, int px, int py, int *ix, int *iy);
int collontilemap(poly *p, tmap *t, int *rx, int *ry);

#endif
