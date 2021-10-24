#ifndef BINDINGS_H
#define BINDINGS_H

void _th_umka_bind(void *umka);
void umfopen(UmkaStackSlot *p, UmkaStackSlot *r);
void umfonttexttoimg(UmkaStackSlot *p, UmkaStackSlot *r);
void umfontload(UmkaStackSlot *p, UmkaStackSlot *r);
void umfontgetyoff(UmkaStackSlot *p, UmkaStackSlot *r);
void umlightmaskclear(UmkaStackSlot *p, UmkaStackSlot *r);
void umlightmaskdraw(UmkaStackSlot *p, UmkaStackSlot *r);
void umspotlightstamp(UmkaStackSlot *p, UmkaStackSlot *r);
void umparticlesdraw(UmkaStackSlot *p, UmkaStackSlot *r);
void umdrawcone(UmkaStackSlot *p, UmkaStackSlot *r);
void umdrawtmap(UmkaStackSlot *p, UmkaStackSlot *r);
void umtmapgetcoll(UmkaStackSlot *p, UmkaStackSlot *r);
void umimgload(UmkaStackSlot *p, UmkaStackSlot *r);
void umimgvalid(UmkaStackSlot *p, UmkaStackSlot *r);
void umimgflipv(UmkaStackSlot *p, UmkaStackSlot *r);
void umimgfliph(UmkaStackSlot *p, UmkaStackSlot *r);
void umimggetdims(UmkaStackSlot *p, UmkaStackSlot *r);
void umimgcrop(UmkaStackSlot *p, UmkaStackSlot *r);
void umimgfromdata(UmkaStackSlot *p, UmkaStackSlot *r);
void umimgcopy(UmkaStackSlot *p, UmkaStackSlot *r);
void umimgsetfilter(UmkaStackSlot *p, UmkaStackSlot *r);
void umgetmouse(UmkaStackSlot *p, UmkaStackSlot *r);
void umispressed(UmkaStackSlot *p, UmkaStackSlot *r);
void umisjustpressed(UmkaStackSlot *p, UmkaStackSlot *r);
void umentdraw(UmkaStackSlot *p, UmkaStackSlot *r);
void umentgetcoll(UmkaStackSlot *p, UmkaStackSlot *r);
void umentysort(UmkaStackSlot *p, UmkaStackSlot *r);
void umauload(UmkaStackSlot *p, UmkaStackSlot *r);
void umauarr(UmkaStackSlot *p, UmkaStackSlot *r);
void umsoundloop(UmkaStackSlot *p, UmkaStackSlot *r);
void umsoundplay(UmkaStackSlot *p, UmkaStackSlot *r);
void umsoundstop(UmkaStackSlot *p, UmkaStackSlot *r);
void umsoundvol(UmkaStackSlot *p, UmkaStackSlot *r);
void umsoundvalidate(UmkaStackSlot *p, UmkaStackSlot *r);
void umraygetcoll(UmkaStackSlot *p, UmkaStackSlot *r);
void umraygettmapcoll(UmkaStackSlot *p, UmkaStackSlot *r);
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
void umCNFGTackRectangle(UmkaStackSlot *p, UmkaStackSlot *r);
void umCNFGChangeWindowTitle(UmkaStackSlot *p, UmkaStackSlot *r);
void umCNFGSetWindowIconData(UmkaStackSlot *p, UmkaStackSlot *r);
void umCNFGTackSegment(UmkaStackSlot *p, UmkaStackSlot *r);
void umCNFGBlitTex(UmkaStackSlot *p, UmkaStackSlot *r);

#endif
