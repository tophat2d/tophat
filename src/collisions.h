#ifndef COLLISIONS_H
#define COLLISIONS_H

int collbyentity(entnode_t *a, entity *e);
int polytopoly(poly *a, poly *b, int r1, int r2, int sx1, int sy1, int sx2, int sy2);
int polytoline(poly *a, int sx, int sy, int ex, int ey, int rot, int six, int siy);
int linetoline(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
int polytopoint(poly *a, int px, int py, int rot, int sx, int sy);

#endif
