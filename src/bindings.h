#ifndef BINDINGS_H
#define BINDINGS_H

#include "../lib/umka/src/umka_api.h"

void umkabind(void *umka);

void umdebug(UmkaStackSlot *p, UmkaStackSlot *r);

void umdebug2(UmkaStackSlot *p, UmkaStackSlot *r);

void umimgload(UmkaStackSlot *p, UmkaStackSlot *r);

void umimgfree(UmkaStackSlot *p, UmkaStackSlot *r);

void umimgsetscale(UmkaStackSlot *p, UmkaStackSlot *r);

void umgetmouse(UmkaStackSlot *p, UmkaStackSlot *r);

void umispressed(UmkaStackSlot *p, UmkaStackSlot *r);

void umisjustpressed(UmkaStackSlot *p, UmkaStackSlot *r);

void umentdraw(UmkaStackSlot *p, UmkaStackSlot *r);

void umgetcoll(UmkaStackSlot *p, UmkaStackSlot *r);

void umsleep(UmkaStackSlot *p, UmkaStackSlot *r);

void umvisualizecam(UmkaStackSlot *p, UmkaStackSlot *r);

void umgettime(UmkaStackSlot *p, UmkaStackSlot *r);

void umdrawtext(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetup(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetBgColor(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetColor(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGClearFrame(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGGetDimensions(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSwapBuffers(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGHandleInput(UmkaStackSlot *p, UmkaStackSlot *r);

void umgetscaling(UmkaStackSlot *p, UmkaStackSlot *r);

#endif
