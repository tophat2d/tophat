#include <GL/gl.h>
#include <stdio.h>

#include "light.h"
#include "misc.h"
#include "rect.h"
#include "../lib/rawdraw/CNFG.h"

extern float CNFGVertDataV[CNFG_BATCH*3];
extern uint32_t CNFGVertDataC[CNFG_BATCH];
extern int CNFGVertPlace;
extern float scaling;

void th_draw_lightcone(th_lightcone *l, th_rect *cam) {
	if (CNFGVertPlace >= CNFG_BATCH-3)
		CNFGFlushRender();

	int camx, camy;
	camx = cam->x - (cam->w / 2);
	camy = cam->y - (cam->h / 2);

	//if (l->px < camx || l->px > camx + cam->w)
		//return;

//	if (l->py < camy || l->py > camy + cam->h)
		//return;

	float *verts = &CNFGVertDataV[CNFGVertPlace*3];
	verts[0] = (l->px - camx) * scaling;           verts[1] = (l->py - camy) * scaling;
	verts[3] = (l->px-l->lenght - camx) * scaling; verts[4] = (l->py-l->width/2 - camy) * scaling;
	verts[6] = (l->px-l->lenght - camx) * scaling; verts[7] = (l->py+l->width/2 - camy) * scaling;
	th_rotate_point(&verts[3], &verts[4], verts[0], verts[1], l->rotation);
	th_rotate_point(&verts[6], &verts[7], verts[0], verts[1], l->rotation);
	uint32_t *colors = &CNFGVertDataC[CNFGVertPlace];
	uint32_t discolor = l->color & 0xffffff22;
	colors[0] = l->color;
	colors[1] = discolor;
	colors[2] = discolor;
	CNFGVertPlace += 3;
}
