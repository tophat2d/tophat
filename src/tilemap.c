
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <CNFG.h>

#include "tophat.h"
#include <GL/gl.h>
#include <chew.h>

extern th_global thg;

extern GLuint gRDShaderProg;
extern GLuint gRDBlitProg;
extern GLuint gRDShaderProgUX;
extern GLuint gRDBlitProgUX;
extern GLuint gRDBlitProgUT;
extern GLuint gRDBlitProgTex;
extern GLuint gRDLastResizeW;
extern GLuint gRDLastResizeH;
#ifdef _WIN32
#define glActiveTexture glActiveTextureCHEW
#endif

void th_tmap_draw(th_tmap *t, th_rect *cam) {
	th_image *a;
	GET_IMAGE(a, t->a.i);

	int camx = cam->x - (cam->w / 2);
	int camy = cam->y - (cam->h / 2);

	if (camx > t->pos.x + t->w*t->scale * t->a.cs.x)
		return;
	if (camy > t->pos.y + t->h*t->scale * t->a.cs.y)
		return;

	int sx = fabs((fabs(t->pos.x)-abs(camx))) / (t->scale * t->a.cs.x);
	int sy = fabs((fabs(t->pos.y)-abs(camy))) / (t->scale * t->a.cs.y);
	int sw = cam->w/(t->scale * t->a.cs.x) + 2;
	int sh = cam->h/(t->scale * t->a.cs.y) + 2;

	if (t->pos.x>=camx)
		sx = 0;
	if (t->pos.y>=camy)
		sy = 0;

	if (sw > t->w)
		sw = t->w - sx;
	if (sh > t->h)
		sh = t->h - sy;

	CNFGFlushRender();

	glUseProgram(gRDBlitProg);
	glUniform4f(gRDBlitProgUX,
		1.f/gRDLastResizeW, -1.f/gRDLastResizeH,
		-0.5f+t->pos.x/(float)gRDLastResizeW, 0.5f-t->pos.y/(float)gRDLastResizeH);
	glUniform1i(gRDBlitProgUT, 0);

	glBindTexture(GL_TEXTURE_2D, a->gltexture);

	for (int i=sx; i < sx+sw; i++) for (int j=sy; j < sy+sh; j++) {
		if (t->cells[j*t->w+i] == 0) continue;
		int cell = t->cells[j * t->w + i];

		const float
			w = t->a.cs.x * thg.scaling * t->scale,
			h = t->a.cs.y * thg.scaling * t->scale,
			x = (t->pos.x - camx) * thg.scaling + w * i,
			y = (t->pos.y - camy) * thg.scaling + h * j;
		const float verts[] = {
			x, y,  x + w, y,      x + w, y + h,
			x, y,  x + w, y + h,  x, y + h	
		};
	
		th_rect r = th_atlas_get_cell(&t->a, th_atlas_nth_coords(&t->a, cell - 1));
		const float tex_verts[] = {
			r.x, r.y,  (r.x + r.w), r.y,          (r.x + r.w), (r.y + r.h),
			r.x, r.y,  (r.x + r.w), (r.y + r.h),  r.x, (r.y + r.h)
		};

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 0, tex_verts);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}
