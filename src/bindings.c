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
	CNFGSetup((char *)&p[0], *(int *)&p[1], *(int *)&p[2]);
}

void umCNFGSetBgColor(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGBGColor = *(uint32_t *)&p;
}

void umCNFGClearFrame(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGClearFrame();
}

void umCNFGSwapBuffers(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGSwapBuffers();
}
