#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>

#include "../lib/rawdraw/CNFG.h"

#include "poly.h"

poly newpoly(int px, int py, int vertnum, ...) {
	va_list valist;
	int *v = malloc(vertnum * sizeof(int) * 2);
	if (v == NULL) {
		printf("could not allocate polygon");
		return;
	}

	va_start(valist, vertnum*2);

	for (int i=0; i < vertnum * 2; i++) {
		v[i] = va_arg(valist, int);
	}

	va_end(valist);

	poly p;
	p.x = px;
	p.y = py;
	p.v = v;
	p.vc = vertnum;

	return p;
}

void freepoly(poly *p) {
	free(p->v);
}

RDPoint *polytordpoint(poly *p, int camx, int camy) {
	RDPoint tr[p->vc];

	for (int i=0; i < p->vc * 2; i += 2) {
		tr[i/2] = { p->x + p->v[i] - camx, p->y + p->v[i + 1] - camy};
	}

	return tr;
}
