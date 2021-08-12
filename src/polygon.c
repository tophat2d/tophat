#include <stdlib.h>
#include <stdarg.h>
#include "../lib/rawdraw/CNFG.h"

#include "tophat.h"

extern float scaling;

RDPoint *_th_poly_to_rdpoint(th_poly *p, int camx, int camy) {
	RDPoint *tr = malloc(sizeof(int) * p->vc * 2);

	for (int i=0; i < p->vc * 2; i += 2) {
		tr[i/2].x = (p->x + p->v[i] - camx) * scaling;
		tr[i/2].y = (p->y + p->v[i+1] - camy) * scaling;
	}

	return tr;
}
