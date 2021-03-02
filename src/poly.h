#ifndef POLY_H
#define POLY_H

#include "../lib/rawdraw/CNFG.h"

// basic polygon structure. The v variable holds all the vertexes. They are like this: [x, y, x, y, ...]
typedef struct {
	int x;
	int y;
	int *v;
	int w;
	int h;
	int vc;
} poly;

poly *newpoly(int px, int py, int vertnum, ...);
void freepoly(poly *p);
RDPoint *polytordpoint(poly *p, int camx, int camy);

#endif
