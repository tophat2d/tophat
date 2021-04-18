#ifndef BINDINGS_H
#define BINDINGS_H

#include "../lib/umka/src/umka_api.h"

void umkabind(void *umka);

void umdebug(UmkaStackSlot *p, UmkaStackSlot *r);

void umdebug2(UmkaStackSlot *p, UmkaStackSlot *r);

void umfopen(UmkaStackSlot *p, UmkaStackSlot *r);

void umimgload(UmkaStackSlot *p, UmkaStackSlot *r);

void umimgfree(UmkaStackSlot *p, UmkaStackSlot *r);

void umimgflipv(UmkaStackSlot *p, UmkaStackSlot *r);

void umimgfliph(UmkaStackSlot *p, UmkaStackSlot *r);

void umimgvalid(UmkaStackSlot *p, UmkaStackSlot *r);

void umimggetdims(UmkaStackSlot *p, UmkaStackSlot *r);

void umimgcrop(UmkaStackSlot *p, UmkaStackSlot *r);

void umgetmouse(UmkaStackSlot *p, UmkaStackSlot *r);

void umispressed(UmkaStackSlot *p, UmkaStackSlot *r);

void umisjustpressed(UmkaStackSlot *p, UmkaStackSlot *r);

void umentdraw(UmkaStackSlot *p, UmkaStackSlot *r);

void umgetcoll(UmkaStackSlot *p, UmkaStackSlot *r);

void umraygetcoll(UmkaStackSlot *p, UmkaStackSlot *r);

void umauload(UmkaStackSlot *p, UmkaStackSlot *r);

void umauarr(UmkaStackSlot *p, UmkaStackSlot *r);

void umsoundloop(UmkaStackSlot *p, UmkaStackSlot *r);

void umsoundplay(UmkaStackSlot *p, UmkaStackSlot *r);

void umsoundstop(UmkaStackSlot *p, UmkaStackSlot *r);

void umsoundvol(UmkaStackSlot *p, UmkaStackSlot *r);

void umsoundvalidate(UmkaStackSlot *p, UmkaStackSlot *r);

void umsleep(UmkaStackSlot *p, UmkaStackSlot *r);

void umvisualizecam(UmkaStackSlot *p, UmkaStackSlot *r);

void umgettime(UmkaStackSlot *p, UmkaStackSlot *r);

void umdrawtext(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetup(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetVSync(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetBgColor(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetColor(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGClearFrame(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGGetDimensions(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSwapBuffers(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGHandleInput(UmkaStackSlot *p, UmkaStackSlot *r);

void umgetscaling(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGTackRectangle(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGChangeWindowTitle(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetWindowIconData(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGTackPoly(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGTackSegment(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGBlitTex(UmkaStackSlot *p, UmkaStackSlot *r);

#endif
