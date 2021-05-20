#include <stdlib.h>
#include <stdarg.h>
#include "../lib/rawdraw/CNFG.h"
#include "polygon.h"
#include "misc.h"

extern float scaling;

void th_new_poly(th_poly *p, int px, int py, int vertnum, ...) {
	int mx, my;
	mx = 0;
	my = 0;
	va_list valist;
	int *v = malloc(vertnum * sizeof(int) * 2);
	if (v == NULL) {
		th_error("could not allocate polygon data.");
		return;
	}

	va_start(valist, vertnum);

	for (int i=0; i < vertnum * 2; i++) {
		v[i] = va_arg(valist, int);

		if (i % 2 == 0 && v[i] > mx)
			mx = v[i];
		if (i % 2 == 1 && v[i] > my)
			my = v[i];
	}

	va_end(valist);

	p->x = px;
	p->y = py;
	p->v = v;
	p->w = mx;
	p->h = my;
	p->vc = vertnum;
}

void th_free_poly(th_poly *p) {
	free(p->v);
}

RDPoint *_th_poly_to_rdpoint(th_poly *p, int camx, int camy) {
	RDPoint *tr = malloc(sizeof(int) * p->vc * 2);

	for (int i=0; i < p->vc * 2; i += 2) {
		tr[i/2].x = (p->x + p->v[i] - camx) * scaling;
		tr[i/2].y = (p->y + p->v[i+1] - camy) * scaling;
	}

	return tr;
}
