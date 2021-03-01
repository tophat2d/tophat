#include "wren/src/include/wren.h"
#include "../src/tophat.h"

void wrnewrect(WrenVM *vm) {
	rect *r;
	r = malloc(sizeof(rect));
	*r = newrect((int)wrenGetSlotDouble(vm, 0), (int)wrenGetSlotDouble(vm, 1), (int)wrenGetSlotDouble(vm, 2), (int)wrenGetSlotDouble(vm, 3));

	wrenSetSlotDouble((double)r);
}

void vrrectsetpot(WrenVM *vm) {
	rect *r;
	r = (rect *)(int)wrenGetSlotDouble(vm, 0);

	r.x = (int)wrenGetSlotDouble(vm, 1);
	r.y = (int)wrenGetSlotDouble(vm, 2);
}
