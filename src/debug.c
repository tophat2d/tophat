#include "rect.h"
#include "../lib/rawdraw/CNFG.h"

void visualizecam(rect cam) {
	CNFGColor(0xff0000ff);
	CNFGTackRectangle(0, 0, cam.w, cam.h);
}
