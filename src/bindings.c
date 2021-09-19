#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <GL/gl.h>

#include <CNFG.h>
#include <chew.h>
#include <umka_api.h>
#include <stb_image.h>

#include "bindings.h"

#include "tophat.h"

#ifdef RELEASE_BUILD
#include "umkalibs.h"
#endif

extern float scaling;
extern int pressed[255];
extern int just_pressed[255];
extern int mousex;
extern int mousey;

extern char *respath;
extern th_sound **sounds;
extern int sound_count;

void _th_umka_bind(void *umka) {
	// etc
	umkaAddFunc(umka, "cfopen", &umfopen);

	umkaAddFunc(umka, "ctexttoimg", &umfonttexttoimg);
	umkaAddFunc(umka, "cfontfree", &umfontfree);
	umkaAddFunc(umka, "cfontload", &umfontload);

	umkaAddFunc(umka, "clightmaskclear", &umlightmaskclear);
	umkaAddFunc(umka, "clightmaskdraw", &umlightmaskdraw);
	umkaAddFunc(umka, "cspotlightstamp", &umspotlightstamp);

	umkaAddFunc(umka, "c_particles_draw", &umparticlesdraw);

	// tilemaps
	umkaAddFunc(umka, "cdrawtmap", &umdrawtmap);
	umkaAddFunc(umka, "ctmapgetcoll", &umtmapgetcoll);

	// images
	umkaAddFunc(umka, "loadimg", &umimgload);
	umkaAddFunc(umka, "deleteimg", &umimgfree);
	umkaAddFunc(umka, "flipvimg", &umimgflipv);
	umkaAddFunc(umka, "fliphimg", &umimgfliph);
	umkaAddFunc(umka, "imgvalid", &umimgvalid);
	umkaAddFunc(umka, "imggetdims", &umimggetdims);
	umkaAddFunc(umka, "imgcrop", &umimgcrop);
	umkaAddFunc(umka, "imgfromdata", &umimgfromdata);
	umkaAddFunc(umka, "imgcopy", &umimgcopy);

	// input
	umkaAddFunc(umka, "cgetmouse", &umgetmouse);
	umkaAddFunc(umka, "cispressed", &umispressed);
	umkaAddFunc(umka, "cisjustpressed", &umisjustpressed);

	// entities
	umkaAddFunc(umka, "centdraw", &umentdraw);
	umkaAddFunc(umka, "cgetcoll", &umentgetcoll);
	umkaAddFunc(umka, "centysort", &umentysort);

	// rays
	umkaAddFunc(umka, "craygetcoll", &umraygetcoll);
	umkaAddFunc(umka, "craygettmapcoll", &umraygettmapcoll);

	// audio
	umkaAddFunc(umka, "cauload", &umauload);
	umkaAddFunc(umka, "cauarr", &umauarr);
	umkaAddFunc(umka, "csoundloop", &umsoundloop);
	umkaAddFunc(umka, "csoundplay", &umsoundplay);
	umkaAddFunc(umka, "csoundstop", &umsoundstop);
	umkaAddFunc(umka, "csoundvol", &umsoundvol);
	umkaAddFunc(umka, "csoundvalidate", &umsoundvalidate);

	// misc
	umkaAddFunc(umka, "visualizecam", &umvisualizecam);
	umkaAddFunc(umka, "gettime", &umgettime);

	// rawdraw
	umkaAddFunc(umka, "drawtext", &umdrawtext);
	umkaAddFunc(umka, "setup", &umCNFGSetup);
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
	umkaAddModule(umka, "misc.um", libs[6]);
	umkaAddModule(umka, "polygon.um", libs[7]);
	umkaAddModule(umka, "rawdraw.um", libs[8]);
	umkaAddModule(umka, "raycast.um", libs[9]);
	umkaAddModule(umka, "rectangle.um", libs[10]);
	umkaAddModule(umka, "tilemap.um", libs[11]);
	umkaAddModule(umka, "tophat.um", libs[12]);
	umkaAddModule(umka, "ui.um", libs[13]);
	umkaAddModule(umka, "vec.um", libs[14]);
	umkaAddModule(umka, "std.um", libs[15]);
	umkaAddModule(umka, "particles.um", libs[16]);
	umkaAddModule(umka, "light.um", libs[17]);
	umkaAddModule(umka, "lerp.um", libs[18]);
	umkaAddModule(umka, "map.um", libs[19]);
	umkaAddModule(umka, "utf8.um", libs[20]);
#endif
}

void umfopen(UmkaStackSlot *p, UmkaStackSlot *r) {
	const char *name = (const char *)p[1].ptrVal;
	const char *mode = (const char *)p[0].ptrVal;

	char path[512];
	strcpy(path, respath);

	FILE *f = fopen(strcat(path, name), mode);
	r->ptrVal = (intptr_t)f;
}

void umfonttexttoimg(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_font *f = (th_font *)p[6].ptrVal;
	uint32_t *runes = (uint32_t *)p[5].ptrVal;
	int runec = p[4].intVal;
	double scale = p[3].realVal;
	uint32_t color = p[2].uintVal;
	int ax = p[1].intVal;
	int ay = p[0].intVal;

	th_image *img = malloc(sizeof(th_image));
	th_str_to_img(img, f, runes, runec, scale, color, ax, ay);
	r->intVal = (intptr_t)img;
}

void umfontfree(UmkaStackSlot *p, UmkaStackSlot *r) {
	free((void *)p[0].ptrVal);
}

void umfontload(UmkaStackSlot *p, UmkaStackSlot *r) {
	char buf[512];
	strcpy(buf, respath);
	strcat(buf, (char *)p[0].ptrVal);

	th_font_load((th_font *)p[1].ptrVal, buf);
}

// sets values of all dots to lightmask's color
void umlightmaskclear(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_lightmask *l = (th_lightmask *)p[0].ptrVal;

	th_lightmask_clear(l);
}

// draws the lightmask
void umlightmaskdraw(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_lightmask *l = (th_lightmask *)p[1].ptrVal;
	th_rect *cam = (th_rect *)p[0].ptrVal;

	th_lightmask_draw(l, cam);
}

// "stamps" the spotlight on the mask
void umspotlightstamp(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_rect *cam = (th_rect *)p[0].ptrVal;
	th_lightmask *l = (th_lightmask *)p[1].ptrVal;
	th_spotlight *s = (th_spotlight *)p[2].ptrVal;

	int x = s->x, y = s->y;
	s->x -= (cam->x - cam->w/2);
	s->y -= (cam->y - cam->h/2);

	th_spotlight_stamp(s, l);

	s->x = x;
	s->y = y;
}

///////////////////////
// particles
void umparticlesdraw(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_particles *emitter = (th_particles *)p[2].ptrVal;
	th_rect *cam = (th_rect *)p[1].ptrVal;
	int t = p[0].intVal;

	th_particles_draw(emitter, *cam, t);
}

///////////////////////
// tilemaps
// draws a tilemap takes a rectangle as a camera and the tilemap itself
void umdrawtmap(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_rect *cam = (th_rect *)p[0].ptrVal;
	th_tmap *t = (th_tmap *)p[1].ptrVal;
	th_tmap_draw(t, cam);
}

// checks, if tilemap collides with entity.
// ent - entity to collide with, t - tilemap, x and y - pointers to ints used to return, where the collision occured
void umtmapgetcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_ent *ent = (th_ent *)p[0].ptrVal;
	th_tmap *t = (th_tmap *)p[1].ptrVal;
	int *y = (int *)p[2].ptrVal;
	int *x = (int *)p[3].ptrVal;
	int *ty = (int *)p[4].ptrVal;
	int *tx = (int *)p[5].ptrVal;

	r->intVal = _th_coll_on_tilemap(&ent->p, t, x, y, tx, ty);
}

///////////////////////
// images
// loads an image at respath + path
void umimgload(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *path = (char *)p[0].ptrVal;

	th_image *img;
	char pathcpy[512];
	strcpy(pathcpy, respath);
	img = th_load_image(strcat(pathcpy, path));
	img->gltexture = CNFGTexImage(img->data, img->w, img->h);

	r[0].ptrVal = (intptr_t)img;
}

// frees an image
void umimgfree(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = (th_image *)p[0].ptrVal;

	th_free_image(img);
}

// checks, if image is correctly loaded
void umimgvalid(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = (th_image *)p[0].ptrVal;
	if (img->data != NULL) {
		r->intVal = 1;
		return;
	}

	r->intVal = 0;
}

// flips image
void umimgflipv(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = (th_image *)p[0].ptrVal;

	th_image_flipv(img);
}

// flips image
void umimgfliph(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = (th_image *)p[0].ptrVal;

	th_image_fliph(img);
}

void umimggetdims(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = (th_image *)p[0].ptrVal;
	int *h = (int *)p[1].ptrVal;
	int *w = (int *)p[2].ptrVal;

	*w = img->w;
	*h = img->h;
}

void umimgcrop(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = (th_image *)p[4].ptrVal;
	int y2 = p[0].intVal;
	int y1 = p[1].intVal;
	int x2 = p[2].intVal;
	int x1 = p[3].intVal;

	th_image_crop(img, x1, y1, x2, y2);
}

// returns a pointer to an image from data
void umimgfromdata(UmkaStackSlot *p, UmkaStackSlot *r) {
	int h = p[0].intVal;
	int w = p[1].intVal;
	uint32_t *data = (uint32_t *)p[2].ptrVal;

	th_image *img = malloc(sizeof(th_image));
	th_image_from_data(img, data, w, h);

	r->ptrVal = (intptr_t)img;
}

void umimgcopy(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *inp = (th_image *)p[0].ptrVal;

	th_image *out = calloc(sizeof(th_image), 1);
	out->w = inp->w;
	out->h = inp->h;
	out->data = calloc(sizeof(uint32_t), out->w * out->h);
	memcpy(out->data, inp->data, sizeof(uint32_t) * out->w * out->h);

	r->ptrVal = (intptr_t)out;
}

///////////////////////
// input
// gets position of mouse cursor
void umgetmouse(UmkaStackSlot *p, UmkaStackSlot *r) {
	int *x = (int *)p[1].ptrVal;
	int *y = (int *)p[0].ptrVal;

	*x = mousex / scaling;
	*y = mousey / scaling;
}

void umispressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r[0].intVal = pressed[keycode];
}

void umisjustpressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r[0].intVal = just_pressed[keycode];
	just_pressed[keycode] = 0;
}

///////////////////////
// entities
// draws an entity
void umentdraw(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_rect *rc = (th_rect *)&p[0];
	th_ent *e = (th_ent *)&p[2];

	if (e->img == 0)
		e->img = NULL;

	th_ent_draw(e, rc);
}

void umentgetcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_ent *scene = (th_ent *)p[0].ptrVal;
	th_ent *e = (th_ent *)p[1].ptrVal;
	int count = p[2].intVal;
	int *iy = (int *)p[3].ptrVal;
	int *ix = (int *)p[4].ptrVal;

	r->intVal = th_ent_getcoll(e, scene, count, ix, iy);
}

int _th_ysort_test(const void *a, const void *b) {
	return *((double *)a + 1) - *((double *)b + 1);
}

void umentysort(UmkaStackSlot *p, UmkaStackSlot *r) {
	void *ents = (void *)p[1].ptrVal;
	int count = p[0].intVal;

	qsort(ents, count, 140, _th_ysort_test);
}

///////////////////////
// audio
void umauload(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = malloc(sizeof(th_sound));
	th_sound_load(s, (char *)p->ptrVal);

	r->ptrVal = (intptr_t)s;
}

// sets array of sounds to be played
void umauarr(UmkaStackSlot *p, UmkaStackSlot *r) {
	int count = p[0].intVal;
	th_sound **auarr = (th_sound **)p[1].ptrVal;

	sound_count = count;
	sounds = auarr;
}

void umsoundloop(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = (th_sound *)p[1].ptrVal;
	s->looping = p[0].intVal;
}

void umsoundplay(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = (th_sound *)p[0].ptrVal;
	s->playing = 1;
}

void umsoundstop(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = (th_sound *)p[0].ptrVal;
	s->playing = 0;
}

void umsoundvol(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = (th_sound *)p[1].ptrVal;
	s->volume = (float)p[0].realVal;
}

// checks, if sound is valid
void umsoundvalidate(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = (th_sound *)p[0].ptrVal;

	if (s == NULL) {
		r[0].intVal = 0;
		return;
	}

	r[0].intVal = 1;
}

///////////////////////
// raycast
void umraygetcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_ent *scene = (th_ent *)p[0].ptrVal;
	th_ray *ra = (th_ray *)p[1].ptrVal;
	int count = p[2].intVal;
	int *iy = (int *)p[3].ptrVal;
	int *ix = (int *)p[4].ptrVal;

	r->intVal = th_ray_getcoll(ra, scene, count, ix, iy);
}

void umraygettmapcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_ray *ra = (th_ray *)p[3].ptrVal;
	th_tmap *t = (th_tmap *)p[2].ptrVal;
	int *ix = (int *)p[1].ptrVal;
	int *iy = (int *)p[0].ptrVal;

	r->intVal = th_ray_to_tilemap(ra, t, ix, iy);
}

///////////////////////
// misc
// draws a rectangle showing, where the camera is
void umvisualizecam(UmkaStackSlot *p, UmkaStackSlot *r) {
	int w = p[2].intVal;
	int h = p[1].intVal;
	int color = p[0].uintVal;

	CNFGColor((uint32_t)color);
	CNFGTackRectangle(0, 0, w * scaling, h * scaling);
}

// gets current time in ms
void umgettime(UmkaStackSlot *p, UmkaStackSlot *r) {
	struct timeval t;
	gettimeofday(&t, NULL);

	r->intVal = (long int)(t.tv_usec);
}

///////////////////////
// rawdraw TODO: hide rawdraw from c lib
// draws text
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

#ifdef _WIN32
	chewInit();
#endif

	if (res) {
		printf("could not initialize rawdraw\n");
		return;
	}
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

// calculates scaling
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
	double y2 = p[0].realVal;
	double x2 = p[1].realVal;
	double y1 = p[2].realVal;
	double x1 = p[3].realVal;

	CNFGTackRectangle(x1 * scaling, y1 * scaling, (x2 + x1) * scaling, (y2 + y1) * scaling);
}

void umCNFGChangeWindowTitle(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGChangeWindowTitle((char *)p->ptrVal);
}

void umCNFGSetWindowIconData(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_error("setasicon is deprecated.");
}

void umCNFGTackPoly(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_poly *pl = (th_poly *)p[0].ptrVal;
	uint32_t color = (uint32_t)p[1].uintVal;

	RDPoint *pr;
	pr = _th_poly_to_rdpoint(pl, 0, 0);
	CNFGColor(color);
	CNFGTackPoly(pr, pl->vc);
	free(pr);
}

void umCNFGTackSegment(UmkaStackSlot *p, UmkaStackSlot *r) {
	double thickness = p[0].realVal;
	int y2 = p[1].intVal;
	int x2 = p[2].intVal;
	int y1 = p[3].intVal;
	int x1 = p[4].intVal;

	CNFGSetLineWidth(thickness * scaling);
	CNFGTackSegment(x1 * scaling, y1 * scaling, x2 * scaling, y2 * scaling);
}

void umCNFGBlitTex(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = (th_image *)p[4].ptrVal;

	int x = p[1].intVal;
	int y = p[0].intVal;

	double s = p[2].realVal;
	int rot = p[3].intVal;

	th_blit_tex(img->gltexture, x * scaling, y * scaling, img->w * s * scaling, img->h * s * scaling, rot);
}
