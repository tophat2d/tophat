#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <GL/gl.h>

#include "tophat.h"
#include "../lib/rawdraw/CNFG.h"
#include "../lib/umka/src/umka_api.h"
#include "../lib/stb/stb_image.h"
//#include "../lib/miniaudio/miniaudio.h"

//#include "audio.h"
#include "bindings.h"
#include "poly.h"

extern float scaling;
extern int *pressed;
extern int *justpressed;
extern int mx;
extern int my;

extern char *respath;
extern sound **sounds;
extern int soundcount;

void umkabind(void *umka) {
	// etc
	umkaAddFunc(umka, "debug", &umdebug);
	umkaAddFunc(umka, "debug2", &umdebug2);

	// images
	umkaAddFunc(umka, "loadimg", &umimgload);
	umkaAddFunc(umka, "deleteimg", &umimgfree);
	umkaAddFunc(umka, "flipvimg", &umimgflipv);
	umkaAddFunc(umka, "fliphimg", &umimgfliph);
	//umkaAddFunc(umka, "imgsetscale", &umimgsetscale);
	//umkaAddFunc(umka, "imgrotate", &umimgrotate);

	// input
	umkaAddFunc(umka, "cgetmouse", &umgetmouse);
	umkaAddFunc(umka, "cispressed", &umispressed);
	umkaAddFunc(umka, "cisjustpressed", &umisjustpressed);

	// entities
	umkaAddFunc(umka, "centdraw", &umentdraw);
	umkaAddFunc(umka, "cgetcoll", &umgetcoll);

	// rays
	umkaAddFunc(umka, "craygetcoll", &umraygetcoll);
	
	// audio
	umkaAddFunc(umka, "cauload", &umauload);
	umkaAddFunc(umka, "cauarr", &umauarr);
	umkaAddFunc(umka, "csoundloop", &umsoundloop);
	umkaAddFunc(umka, "csoundplay", &umsoundplay);
	umkaAddFunc(umka, "csoundstop", &umsoundstop);
	umkaAddFunc(umka, "csounddelete", &umsounddelete);
	umkaAddFunc(umka, "csoundvol", &umsoundvol);

	// misc
	umkaAddFunc(umka, "sleep", &umsleep);
	umkaAddFunc(umka, "visualizecam", &umvisualizecam);
	umkaAddFunc(umka, "gettime", &umgettime);

	// rawdraw
	umkaAddFunc(umka, "drawtext", &umdrawtext);
	umkaAddFunc(umka, "setup", &umCNFGSetup);
	umkaAddFunc(umka, "setvsync", &umCNFGSetVSync);
	umkaAddFunc(umka, "setbgcolor", &umCNFGSetBgColor);
	umkaAddFunc(umka, "setcolor", &umCNFGSetColor);
	umkaAddFunc(umka, "clearframe", &umCNFGClearFrame);
	umkaAddFunc(umka, "getdimensions", &umCNFGGetDimensions);
	umkaAddFunc(umka, "swapbuffers", &umCNFGSwapBuffers);
	umkaAddFunc(umka, "handleinput", &umCNFGHandleInput);
	umkaAddFunc(umka, "updatescaling", &umgetscaling);
	umkaAddFunc(umka, "drawrect", &umCNFGTackRectangle);
	umkaAddFunc(umka, "setwindowtitle", &umCNFGChangeWindowTitle);
	umkaAddFunc(umka, "iconset", &umCNFGSetWindowIconData);
	umkaAddFunc(umka, "cdrawpoly", &umCNFGTackPoly);
	umkaAddFunc(umka, "drawsegment", &umCNFGTackSegment);
	umkaAddFunc(umka, "cdrawimage", &umCNFGBlitTex);
}

// etc
void umdebug(UmkaStackSlot *p, UmkaStackSlot *r) {
	//auplay(dc);	
}

void umdebug2(UmkaStackSlot *p, UmkaStackSlot *r) {
	//dc = auload("test.wav");
	//auinit();
}

// images
void umimgload(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *path = (char *)p[0].ptrVal;

	image *img;
	char pathcpy[512];
	strcpy(pathcpy, respath);
	img = loadimage(strcat(pathcpy, path));
	rdimg(img, scaling);
	img->tex = CNFGTexImage(img->rdimg, img->w, img->h);

	r[0].ptrVal = (intptr_t)img;
	stbi_image_free(img->raw);
}
void umimgfree(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[0].ptrVal;

	free(img->rdimg);
	stbi_image_free(img->raw);
	free(img);
}
void umimgflipv(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[0].ptrVal;

	flipv(img);
	glDeleteTextures(1, &img->tex);
	img->tex = CNFGTexImage(img->rdimg, img->w, img->h);
}
void umimgfliph(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[0].ptrVal;

	fliph(img);
	glDeleteTextures(1, &img->tex);
	img->tex = CNFGTexImage(img->rdimg, img->w, img->h);
}
/*void umimgsetscale(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[0].ptrVal;
	double x = p[1].realVal;
	double y = p[2].realVal;

	img->scalex = x;
	img->scaley = y;
}
void umimgrotate(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[0].ptrVal;
	int rot = p[1].intVal;

	img->rot = rot;
}*/

// input
void umgetmouse(UmkaStackSlot *p, UmkaStackSlot *r) {
	int *x = (int *)p[1].ptrVal;
	int *y = (int *)p[0].ptrVal;

	*x = mx / scaling;
	*y = my / scaling;
}

void umispressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	//printf("%d\n", pressed[keycode]);

	r[0].intVal = pressed[keycode];
}

void umisjustpressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r[0].intVal = justpressed[keycode];
	justpressed[keycode] = 0;
}

// entities
void umentdraw(UmkaStackSlot *p, UmkaStackSlot *r) {
	rect *rc = (rect *)&p[0];
	entity *e = (entity *)(rc + sizeof(UmkaStackSlot *)/sizeof(UmkaStackSlot)); // this is weird solution, but it seems to work for now. TODO

	if (e->img == 0) {
		e->img = NULL;
	}

	draw(e, rc);
}

void umgetcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	entity **scene = (entity **)p[0].ptrVal;
	entity *e = (entity *)p[1].ptrVal;
	int count = p[2].intVal;
	int coll;

	//printf("%d, %X, %d\n", e->p->x, e->color, sizeof(poly *));

	for (int i=0; i < count; i++) {
		if (e->id == scene[i]->id) {
			continue;
		}

		if (e->p->x > scene[i]->p->x + scene[i]->p->w) {
			//printf("exit due to bounding box %d\n", rand());
			continue;
		}

		if (e->p->y > scene[i]->p->y + scene[i]->p->h) {
			//printf("exit due to bounding box %d\n", rand());
			continue;
		}

		if (e->p->w + e->p->x < scene[i]->p->x) {
			//printf("exit due to bounding box %d\n", rand());
			continue;
		}

		if (e->p->h + e->p->y < scene[i]->p->y) {
			//printf("exit due to bounding box %d\n", rand());
			continue;
		}

		coll = polytopoly(scene[i]->p, e->p);
		if (coll) {
			r->intVal = scene[i]->id;
			return;
		}

		coll = polytopoly(e->p, scene[i]->p);
		if (coll) {
			r->intVal = scene[i]->id;
			return;
		}
	}
	r->intVal = 0;
}

// audio
void umauload(UmkaStackSlot *p, UmkaStackSlot *r) {
	sound *s = auload((char *)p->ptrVal);

	r->ptrVal = (intptr_t)s;
}

void umauarr(UmkaStackSlot *p, UmkaStackSlot *r) {
	int count = p[0].intVal;
	sound **auarr = (sound **)p[1].ptrVal;

	soundcount = count;
	sounds = auarr;
}

void umsoundloop(UmkaStackSlot *p, UmkaStackSlot *r) {
	sound *s = (sound *)p[1].ptrVal;
	s->looping = p[0].intVal;
}

void umsoundplay(UmkaStackSlot *p, UmkaStackSlot *r) {
	sound *s = (sound *)p[0].ptrVal;
	s->playing = 1;
}

void umsoundstop(UmkaStackSlot *p, UmkaStackSlot *r) {
	sound *s = (sound *)p[0].ptrVal;
	s->playing = 0;
}

void umsounddelete(UmkaStackSlot *p, UmkaStackSlot *r) {
	sound *s = (sound *)p[0].ptrVal;

	ma_decoder_uninit(&s->decoder);

	free(s);
}

void umsoundvol(UmkaStackSlot *p, UmkaStackSlot *r) {
	sound *s = (sound *)p[1].ptrVal;
	s->volume = (float)p[0].realVal;
}

// rays
void umraygetcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	entity **scene = (entity **)p[0].ptrVal;
	ray *ra = (ray *)p[1].ptrVal;
	int count = p[2].intVal;
	int coll;

	int rx, ry;
	rx = ra->x;
	ry = ra->y - ra->l;

	rotatepoint(&rx, &ry, ra->x, ra->y, ra->r);
	
	for (int i=0; i < count; i++) {
		if (ra->x > scene[i]->p->x + scene[i]->p->w) {
			continue;
		}

		if (ra->y > scene[i]->p->y + scene[i]->p->h) {
			continue;
		}

		if (rx < scene[i]->p->x) {
			continue;
		}

		if (ry < scene[i]->p->y) {
			continue;
		}

		coll = polytoline(scene[i]->p, ra->x, ra->y, rx, ry);
		if (coll) {
			r->intVal = scene[i]->id;
			return;
		}
	}
	r->intVal = 0;
}


// misc
void umsleep(UmkaStackSlot *p, UmkaStackSlot *r) {
	int t = p[0].intVal;

	slp(t);
}

void umvisualizecam(UmkaStackSlot *p, UmkaStackSlot *r) {
	int w = p[2].intVal;
	int h = p[1].intVal;
	int color = p[0].uintVal;

	CNFGColor((uint32_t)color);
	CNFGTackRectangle(0, 0, w * scaling, h * scaling);
}

void umgettime(UmkaStackSlot *p, UmkaStackSlot *r) {
	struct timeval t;
	gettimeofday(&t, NULL);

	r->intVal = (long int)(t.tv_usec);
}

// rawdraw
void umdrawtext(UmkaStackSlot *p, UmkaStackSlot *r) {
	double size = p[0].realVal;
	uint32_t color = (uint32_t)p[1].uintVal;
	int y = (int)p[2].intVal;
	int x = (int)p[3].intVal;
	char *text = (char *)p[4].ptrVal;

	CNFGPenX = x * scaling;
	CNFGPenY = y * scaling;
	CNFGColor(color);
	CNFGSetLineWidth(0.6 * scaling);
	CNFGDrawText(text, size * scaling);
}

void umCNFGSetup(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *title = (char *)p[2].ptrVal;
	int w = p[1].intVal;
	int h = p[0].intVal;

	int res = CNFGSetup(title, w, h);

	//auinit();

	if (res) {
		printf("could not initialize rawdraw\n");
		return;
	}
}

void umCNFGSetVSync(UmkaStackSlot *p, UmkaStackSlot *r) {
}

void umCNFGSetBgColor(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGBGColor = (uint32_t)p[0].uintVal;
}

void umCNFGSetColor(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGColor((uint32_t)p[0].uintVal);
}

void umCNFGClearFrame(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGClearFrame();
}

void umCNFGGetDimensions(UmkaStackSlot *p, UmkaStackSlot *r) {
	int *w = (int *)p[1].ptrVal;
	int *h = (int *)p[0].ptrVal;
	short ws, hs;

	CNFGGetDimensions(&ws, &hs);

	*w = ws;
	*h = hs;
}

void umCNFGSwapBuffers(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGSwapBuffers();
}

void umCNFGHandleInput(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGHandleInput();
}

void umgetscaling(UmkaStackSlot *p, UmkaStackSlot *r) {
	int camh = p[0].intVal;
	int camw = p[1].intVal;
	int h = p[2].intVal;
	int w = p[3].intVal;

	if ((float)w/camw < (float)h/camh) {
		scaling = ((float)w/camw);
	} else {
		scaling = ((float)h/camh);
	}
}

void umCNFGTackRectangle(UmkaStackSlot *p, UmkaStackSlot *r) {
	int y2 = p[0].intVal;
	int x2 = p[1].intVal;
	int y1 = p[2].intVal;
	int x1 = p[3].intVal;

	CNFGTackRectangle(x1 * scaling, y1 * scaling, (x2 + x1) * scaling, (y2 + y1) * scaling);
}

void umCNFGChangeWindowTitle(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGChangeWindowTitle((char *)p->ptrVal);
}

void umCNFGSetWindowIconData(UmkaStackSlot *p, UmkaStackSlot *r) {
#ifndef _WIN32
	image *img = (image *)p[0].ptrVal;

	CNFGSetWindowIconData(img->w, img->h, img->rdimg);
#else
	printf("\033[31merror\033[0m: can't set window icon on windows\n");
#endif
}

void umCNFGTackPoly(UmkaStackSlot *p, UmkaStackSlot *r) {
	poly *pl = (poly *)p[0].ptrVal;
	uint32_t color = (uint32_t)p[1].uintVal;

	RDPoint *pr;
	pr = polytordpoint(pl, 0, 0);
	CNFGColor(color);
	CNFGTackPoly(pr, pl->vc);
	free(pr);
}

void umCNFGTackSegment(UmkaStackSlot *p, UmkaStackSlot *r) {
	int y2 = p[0].intVal;
	int x2 = p[1].intVal;
	int y1 = p[2].intVal;
	int x1 = p[3].intVal;

	CNFGTackSegment(x1 * scaling, y1 * scaling, x2 * scaling, y2 * scaling);
}

void umCNFGBlitTex(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[4].ptrVal;

	int x = p[1].intVal;
	int y = p[0].intVal;

	double s = p[2].realVal;
	
	int rot = p[3].intVal;
	
	CNFGBlitTex(img->tex, x * scaling, y * scaling, img->w * s * scaling, img->h * s * scaling, rot);
}
