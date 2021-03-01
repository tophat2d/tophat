#ifndef BINDINGS_H
#define BINDINGS_H

#include "../lib/umka/src/umka_api.h"

void bind(void *umka);

void umCNFGSetup(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSetBgColor(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGClearFrame(UmkaStackSlot *p, UmkaStackSlot *r);

void umCNFGSwapBuffers(UmkaStackSlot *p, UmkaStackSlot *r);

#endif
