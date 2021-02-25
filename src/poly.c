#include <stdarg.h>

#include "../../rawdraw/CNFG.h"

poly newpoly(int px, int py, int vertnum, ...) {
	va_list valist;
	int *v = malloc(vertnum * sizeof(int) * 2);
	if (v == NULL) {
		printf("could not allocate polygon");
		return NULL;
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

RDPoint polytordpoint(int *v, int vc) {
	RDPoint tr[vc];

	for (int i=0; i < vc * 2; i += 2) {
		tr[i/2] = { v[i], v[i + 1] };
	}

	return tr;
}
