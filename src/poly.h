#ifndef POLY_H
#define POLY_H

// basic polygon structure. the v variable holds all the vertices. they are like this: [x, y, x, y, ...]
typedef struct {
	int x;
	int y;
	int *v;
	int vc;
} poly;

poly newpoly(int px, int py, int vertnum, ...);
void freepoly(poly *p);

#endif
