#include <stdlib.h>
#include <stdarg.h>
#include <CNFG.h>

#include "tophat.h"

extern float scaling;

// TODO: don't do this on the heap
RDPoint *_th_poly_to_rdpoint(th_poly *p, int camx, int camy) {
	RDPoint *tr = malloc(sizeof(RDPoint) * p->vc);

	for (int i=0; i < p->vc * 2; i += 2) {
		tr[i/2].x = (p->x + p->v[i] - camx) * scaling;
		tr[i/2].y = (p->y + p->v[i+1] - camy) * scaling;
	}

	return tr;
}
