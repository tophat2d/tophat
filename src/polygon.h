#ifndef POLYGON_H
#define POLYGON_H

#include "../lib/rawdraw/CNFG.h"

typedef struct {
	int x;
	int y;
	int *v;
	int w;
	int h;
	int vc;
} th_poly;

void th_new_poly(th_poly *p, int px, int py, int vertnum, ...);
void th_free_poly(th_poly *p);
RDPoint *_th_poly_to_rdpoint(th_poly *p, int camx, int camy);

#endif
