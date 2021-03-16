#ifndef COLLISIONS_H
#define COLLISIONS_H

int collbyentity(entnode_t *a, entity *e);
int polytopoly(poly *a, poly *b);
int polytoline(poly *a, int sx, int sy, int ex, int ey);
int linetoline(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
int polytopoint(poly *a, int px, int py);

#endif
