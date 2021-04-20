#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <GL/gl.h>

#include "tophat.h"
#include "../lib/rawdraw/CNFG.h"
#include "../lib/umka/src/umka_api.h"
#include "../lib/stb/stb_image.h"

#include "bindings.h"
#include "poly.h"
#include "tilemap.h"
#ifdef RELEASE_BUILD
#include "umkalibs.h"
#endif

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
	umkaAddFunc(umka, "cfopen", &umfopen);

	// tilemaps
	umkaAddFunc(umka, "cdrawtmap", &umdrawtmap);

	// images
	umkaAddFunc(umka, "loadimg", &umimgload);
	umkaAddFunc(umka, "deleteimg", &umimgfree);
	umkaAddFunc(umka, "flipvimg", &umimgflipv);
	umkaAddFunc(umka, "fliphimg", &umimgfliph);
	umkaAddFunc(umka, "imgvalid", &umimgvalid);
	umkaAddFunc(umka, "imggetdims", &umimggetdims);
	umkaAddFunc(umka, "imgcrop", &umimgcrop);

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
	umkaAddFunc(umka, "csoundvol", &umsoundvol);
	umkaAddFunc(umka, "csoundvalidate", &umsoundvalidate);

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

#ifdef RELEASE_BUILD
	umkaAddModule(umka, "animation.um", libs[0]);
	umkaAddModule(umka, "audio.um", libs[1]);
	umkaAddModule(umka, "csv.um", libs[2]);
	umkaAddModule(umka, "entity.um", libs[3]);
	umkaAddModule(umka, "image.um", libs[4]);
	umkaAddModule(umka, "input.um", libs[5]);
	umkaAddModule(umka, "map.um", libs[6]);
	umkaAddModule(umka, "misc.um", libs[7]);
	umkaAddModule(umka, "polygon.um", libs[8]);
	umkaAddModule(umka, "rawdraw.um", libs[9]);
	umkaAddModule(umka, "raycast.um", libs[10]);
	umkaAddModule(umka, "rectangle.um", libs[11]);
	umkaAddModule(umka, "tilemap.um", libs[12]);
	umkaAddModule(umka, "tophat.um", libs[13]);
	umkaAddModule(umka, "ui.um", libs[14]);
	umkaAddModule(umka, "vec.um", libs[15]);
	umkaAddModule(umka, "std.um", libs[16]);
#endif
}

// etc
void umdebug(UmkaStackSlot *p, UmkaStackSlot *r) {
	//auplay(dc);	
}

void umdebug2(UmkaStackSlot *p, UmkaStackSlot *r) {
	//dc = auload("test.wav");
	//auinit();
}

void umfopen(UmkaStackSlot *p, UmkaStackSlot *r) {
	const char *name = (const char *)p[1].ptrVal;
	const char *mode = (const char *)p[0].ptrVal;

	char path[512];
	strcpy(respath, path);

	FILE *f = fopen(strcat(path, name), mode);
	r->ptrVal = (intptr_t)f;
}

// tilemaps
void umdrawtmap(UmkaStackSlot *p, UmkaStackSlot *r) {
	rect *cam = (rect *)p[0].ptrVal;
	tmap *t = (tmap *)p[1].ptrVal;
	tmapdraw(t, cam);
}

// images
void umimgload(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *path = (char *)p[0].ptrVal;

	image *img;
	char pathcpy[512];
	strcpy(pathcpy, respath);
	img = loadimage(strcat(pathcpy, path));
	img->tex = CNFGTexImage(img->rdimg, img->w, img->h);

	r[0].ptrVal = (intptr_t)img;
}

void umimgfree(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[0].ptrVal;

	free(img->rdimg);
	free(img);
}

void umimgvalid(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[0].ptrVal;
	if (img->rdimg != NULL) {
		r->intVal = 1;
		return;
	}

	r->intVal = 0;
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

void umimggetdims(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[0].ptrVal;
	int *h = (int *)p[1].ptrVal;
	int *w = (int *)p[2].ptrVal;

	*w = img->w;
	*h = img->h;
}

void umimgcrop(UmkaStackSlot *p, UmkaStackSlot *r) {
	image *img = (image *)p[4].ptrVal;
	int y2 = p[0].intVal;
	int y1 = p[1].intVal;
	int x2 = p[2].intVal;
	int x1 = p[3].intVal;

	imgcrop(img, x1, y1, x2, y2);
	glDeleteTextures(1, &img->tex);
	img->tex = CNFGTexImage(img->rdimg, img->w, img->h);
}

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
	entity *e = (entity *)&p[2]; // this is weird solution, but it seems to work for now. TODO

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

	for (int i=0; i < count; i++) {
		if (e->id == scene[i]->id)
			continue;

		if (e->p->x > scene[i]->p->x + scene[i]->p->w)
			continue;

		if (e->p->y > scene[i]->p->y + scene[i]->p->h)
			continue;

		if (e->p->w + e->p->x < scene[i]->p->x)
			continue;

		if (e->p->h + e->p->y < scene[i]->p->y)
			continue;

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

void umsoundvol(UmkaStackSlot *p, UmkaStackSlot *r) {
	sound *s = (sound *)p[1].ptrVal;
	s->volume = (float)p[0].realVal;
}

void umsoundvalidate(UmkaStackSlot *p, UmkaStackSlot *r) {
	sound *s = (sound *)p[0].ptrVal;

	if (s == NULL) {
		r[0].intVal = 0;
		return;
	}

	r[0].intVal = 1;
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

	RotatePoint(&rx, &ry, ra->x, ra->y, ra->r);
	
	rect rr = newrect(ra->x, ra->y, rx, ry);

	if (rx < ra->x) {
		rr.x = rx;
		rr.w = ra->x;
	}

	if (ry < ra->y) {
		rr.y = ry;
		rr.h = ra->y;
	}

	//CNFGColor(0x8800ffff);
	//CNFGTackRectangle((rr.x+40) * scaling, (rr.y+40) * scaling, (rr.w + 40) * scaling, (rr.h + 40) * scaling);

	for (int i=0; i < count; i++) {

		if (rr.x > scene[i]->p->x + scene[i]->p->w)
			continue;

		if (rr.y > scene[i]->p->y + scene[i]->p->h)
			continue;

		if (rr.w < scene[i]->p->x)
			continue;

		if (rr.h < scene[i]->p->y)
			continue;

		coll = polytoline(scene[i]->p, ra->x, ra->y, rx, ry);
		if (coll) {
			r->intVal = scene[i]->id;
			return;
		}

		coll = polytopoint(scene[i]->p, rx, ry);
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
	CNFGSetLineWidth(0.6 * scaling * size);
	CNFGDrawText(text, size * scaling);
}

void umCNFGSetup(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *title = (char *)p[2].ptrVal;
	int w = p[1].intVal;
	int h = p[0].intVal;

	int res = CNFGSetup(title, w, h);

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
	errprint("can't set window icon on this platform");
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
