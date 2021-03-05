#ifndef BINDINGS_H
#define BINDINGS_H

#include "../lib/umka/src/umka_api.h"

void bind(void *umka);

void umdebug(UmkaStackSlot *p, UmkaStackSlot *r);

void umentdraw(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetup(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetBgColor(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetColor(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGClearFrame(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGGetDimensions(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSwapBuffers(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGHandleInput(UmkaStackSlot *p, UmkaStackSlot *r);

#endif
