#include <stdio.h>

#include "tophat.h"
#include "../lib/rawdraw/CNFG.h"
#include "../lib/umka/src/umka_api.h"

#include "bindings.h"

void bind(void *umka) {
	umkaAddFunc(umka, "cnfgsetup", &umCNFGSetup);
	umkaAddFunc(umka, "cnfgsetbgcolor", &umCNFGSetBgColor);
	umkaAddFunc(umka, "cnfgclearframe", &umCNFGClearFrame);
	umkaAddFunc(umka, "cnfgswapbuffers", &umCNFGSwapBuffers);
}

void umCNFGSetup(UmkaStackSlot *p, UmkaStackSlot *r) {
	//char *title = (char *)p[0].ptrVal;
	int w = (int)p[1].intVal;
	int h = (int)p[2].intVal;

	int res = CNFGSetup("title", w, h);

	if (res) {
		printf("could not initialize rawdraw\n");
	}
}

void umCNFGSetBgColor(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGBGColor = (uint32_t)p[0].uintVal;
}

void umCNFGClearFrame(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGClearFrame();
}

void umCNFGSwapBuffers(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGSwapBuffers();
}
