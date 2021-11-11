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

extern th_global thg;

void _th_umka_bind(void *umka) {
	// etc
	umkaAddFunc(umka, "cfopen", &umfopen);

	umkaAddFunc(umka, "ctexttoimg", &umfonttexttoimg);
	umkaAddFunc(umka, "cfontload", &umfontload);
	umkaAddFunc(umka, "getYOff", &umfontgetyoff);

	umkaAddFunc(umka, "clightmaskclear", &umlightmaskclear);
	umkaAddFunc(umka, "clightmaskdraw", &umlightmaskdraw);
	umkaAddFunc(umka, "cspotlightstamp", &umspotlightstamp);

	umkaAddFunc(umka, "c_particles_draw", &umparticlesdraw);

	// tilemaps
	umkaAddFunc(umka, "cdrawtmap", &umdrawtmap);
	umkaAddFunc(umka, "ctmapgetcoll", &umtmapgetcoll);

	// images
	umkaAddFunc(umka, "loadimg", &umimgload);
	umkaAddFunc(umka, "flipvimg", &umimgflipv);
	umkaAddFunc(umka, "fliphimg", &umimgfliph);
	umkaAddFunc(umka, "imgvalid", &umimgvalid);
	umkaAddFunc(umka, "imggetdims", &umimggetdims);
	umkaAddFunc(umka, "imgcrop", &umimgcrop);
	umkaAddFunc(umka, "imgfromdata", &umimgfromdata);
	umkaAddFunc(umka, "imgcopy", &umimgcopy);
	umkaAddFunc(umka, "imgsetfilter", &umimgsetfilter);

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
	umkaAddFunc(umka, "csoundloop", &umsoundloop);
	umkaAddFunc(umka, "csoundplay", &umsoundplay);
	umkaAddFunc(umka, "csoundstop", &umsoundstop);
	umkaAddFunc(umka, "csoundvol", &umsoundvol);
	umkaAddFunc(umka, "csoundvalidate", &umsoundvalidate);

	// misc
	umkaAddFunc(umka, "visualizecam", &umvisualizecam);
	umkaAddFunc(umka, "getTime", &umgettime);

	// rawdraw
	umkaAddFunc(umka, "drawText", &umdrawtext);
	umkaAddFunc(umka, "wsetup", &umCNFGSetup);
	umkaAddFunc(umka, "setBgColor", &umCNFGSetBgColor);
	umkaAddFunc(umka, "setColor", &umCNFGSetColor);
	umkaAddFunc(umka, "clearframe", &umCNFGClearFrame);
	umkaAddFunc(umka, "getdimensions", &umCNFGGetDimensions);
	umkaAddFunc(umka, "swapbuffers", &umCNFGSwapBuffers);
	umkaAddFunc(umka, "handleinput", &umCNFGHandleInput);
	umkaAddFunc(umka, "updatescaling", &umgetscaling);
	umkaAddFunc(umka, "drawRect", &umCNFGTackRectangle);
	umkaAddFunc(umka, "setwindowtitle", &umCNFGChangeWindowTitle);
	umkaAddFunc(umka, "iconset", &umCNFGSetWindowIconData);
	umkaAddFunc(umka, "drawSegment", &umCNFGTackSegment);
	umkaAddFunc(umka, "cdrawimage", &umCNFGBlitTex);

#ifdef RELEASE_BUILD
	int index = 0;
	umkaAddModule(umka, "anim.um", libs[index++]);
	umkaAddModule(umka, "audio.um", libs[index++]);
	umkaAddModule(umka, "csv.um", libs[index++]);
	umkaAddModule(umka, "ent.um", libs[index++]);
	umkaAddModule(umka, "image.um", libs[index++]);
	umkaAddModule(umka, "input.um", libs[index++]);
	umkaAddModule(umka, "misc.um", libs[index++]);
	umkaAddModule(umka, "canvas.um", libs[index++]);
	umkaAddModule(umka, "ray.um", libs[index++]);
	umkaAddModule(umka, "rect.um", libs[index++]);
	umkaAddModule(umka, "tilemap.um", libs[index++]);
	umkaAddModule(umka, "window.um", libs[index++]);
	umkaAddModule(umka, "ui.um", libs[index++]);
	umkaAddModule(umka, "std.um", libs[index++]);
	umkaAddModule(umka, "particles.um", libs[index++]);
	umkaAddModule(umka, "light.um", libs[index++]);
	umkaAddModule(umka, "lerp.um", libs[index++]);
	umkaAddModule(umka, "map.um", libs[index++]);
	umkaAddModule(umka, "utf8.um", libs[index++]);
	umkaAddModule(umka, "font.um", libs[index++]);
	umkaAddModule(umka, "th.um", libs[index++]);
	umkaAddModule(umka, "signal.um", libs[index++]);
#endif
}

void umfopen(UmkaStackSlot *p, UmkaStackSlot *r) {
	const char *name = (const char *)p[1].ptrVal;
	const char *mode = (const char *)p[0].ptrVal;

	char path[512];
	strcpy(path, thg.respath);

	FILE *f = fopen(strcat(path, name), mode);
	r->ptrVal = (intptr_t)f;
}

void umfonttexttoimg(UmkaStackSlot *p, UmkaStackSlot *r) {
	if (p[5].uintVal-1 >= thg.font_count) {
		th_error("Invalid font\n");
		return;
	}
	th_font *f = thg.fonts[p[5].uintVal-1];
	uint32_t *runes = (uint32_t *)p[4].ptrVal;
	uu runec = p[3].intVal;
	fu scale = p[2].real32Val;
	uint32_t color = p[1].uintVal;
	th_vf2 spacing = *(th_vf2 *)&p[0];

	if (thg.image_count >= MAX_IMAGES - 1) {
		th_error("Too many images. Create an issue.");
		return;
	}
	th_image *img = thg.images[thg.image_count++] = calloc(sizeof(th_image), 1);
	th_str_to_img(img, f, runes, runec, scale, color, spacing);
	r->intVal = thg.image_count;
}

void umfontload(UmkaStackSlot *p, UmkaStackSlot *r) {
	char buf[512];
	strcpy(buf, thg.respath);
	strcat(buf, (char *)p[0].ptrVal);

	if (thg.font_count >= MAX_FONTS - 1) {
		th_error("Too many fonts. Create an issue.");
		return;
	}
	th_font *f = thg.fonts[thg.font_count++] = calloc(sizeof(th_font), 1);
	th_font_load(f, buf);
	r->intVal = thg.font_count;
}

void umfontgetyoff(UmkaStackSlot *p, UmkaStackSlot *r) {
	if (p[1].uintVal-1 >= thg.font_count) {
		th_error("Invalid font %d\n", p[1].intVal);
		return;
	}
	th_font *f = thg.fonts[p[1].uintVal-1];
	uint32_t rune = p[0].uintVal;

	int out, t;
	stbtt_GetCodepointBitmapBox(f->info, rune, 1, 1, &t, &out, &t, &t);
	r->intVal = out;
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

	int x = s->pos.x, y = s->pos.x;
	s->pos.x -= (cam->x - cam->w/2);
	s->pos.y -= (cam->y - cam->h/2);

	th_spotlight_stamp(s, l);

	s->pos.x = x;
	s->pos.y = y;
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
	uu *vert = (uu *)p[2].ptrVal;
	th_vf2 *tc = (th_vf2 *)p[3].ptrVal;

	r->intVal = _th_coll_on_tilemap(ent, t, vert, tc);
}

///////////////////////
// images
// loads an image at respath + path
void umimgload(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *path = (char *)p[0].ptrVal;

	if (thg.image_count >= MAX_IMAGES - 1) {
		th_error("Too many images. Create an issue.");
		return;
	}
	char pathcpy[512];
	strcpy(pathcpy, thg.respath);
	th_image *img = thg.images[thg.image_count++] = th_load_image(strcat(pathcpy, path));
	img->gltexture = th_gen_texture(img->data, img->dm, img->filter);

	r[0].ptrVal = thg.image_count;
}

// checks, if image is correctly loaded
void umimgvalid(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img;
	GET_IMAGE(img, p[0].intVal);
	if (img->data != NULL) {
		r->intVal = 1;
		return;
	}

	r->intVal = 0;
}

// flips image
void umimgflipv(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img;
	GET_IMAGE(img, p[0].intVal);

	img->flipv = !img->flipv;
}

// flips image
void umimgfliph(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img;
	GET_IMAGE(img, p[0].intVal);

	img->fliph = !img->fliph;
}

void umimggetdims(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img;
	GET_IMAGE(img, p[0].intVal);
	th_vf2 *out = (th_vf2 *)p[1].ptrVal;

	if (!img)
		return;

	*out = img->dm;
}

void umimgcrop(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img;
	GET_IMAGE(img, p[2].intVal);
	th_vf2 tl = *(th_vf2 *)&p[1];
	th_vf2 br = *(th_vf2 *)&p[0];

	img->crop = (th_rect){tl.x, tl.y, br.x, br.y};
}

// returns a pointer to an image from data
void umimgfromdata(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 dm = *(th_vf2 *)&p[0];
	uint32_t *data = (uint32_t *)p[1].ptrVal;

	if (thg.image_count >= MAX_IMAGES - 1) {
		th_error("Too many images. Create an issue.");
		return;
	}
	th_image *img = thg.images[thg.image_count++] = calloc(sizeof(th_image), 1);
	th_image_from_data(img, data, dm);

	r->ptrVal = thg.image_count;
}

void umimgcopy(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *inp;
	GET_IMAGE(inp, p[0].intVal);

	if (thg.image_count >= MAX_IMAGES - 1) {
		th_error("Too many images. Create an issue.");
		return;
	}
	th_image *out = thg.images[thg.image_count++] = calloc(sizeof(th_image), 1);
	out->dm = inp->dm;
	out->data = calloc(sizeof(uint32_t), out->dm.w * out->dm.h);
	memcpy(out->data, inp->data, sizeof(uint32_t) * out->dm.w * out->dm.h);

	r->ptrVal = thg.image_count;
}

void umimgsetfilter(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img;
	GET_IMAGE(img, p[1].intVal);
	int filter = p[0].intVal;

	th_image_set_filter(img, filter);
}

///////////////////////
// input
// gets position of mouse cursor
void umgetmouse(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 *out = (th_vf2 *)p[0].ptrVal;
	out->x = thg.mouse.x / thg.scaling;
	out->y = thg.mouse.y / thg.scaling;
}

void umispressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r[0].intVal = thg.pressed[keycode];
}

void umisjustpressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r[0].intVal = thg.just_pressed[keycode];
	thg.just_pressed[keycode] = 0;
}

///////////////////////
// entities
// draws an entity
void umentdraw(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_rect *rc = (th_rect *)p[0].ptrVal;
	th_ent *e = (th_ent *)p[1].ptrVal;
	th_ent_draw(e, rc);
}

void umentgetcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_ent **scene = (th_ent **)p[0].ptrVal;
	th_ent *e = (th_ent *)p[1].ptrVal;
	int count = p[2].intVal;
	uu maxColls = p[3].ptrVal;
	uu *collC = (uu *)p[4].ptrVal;
	th_coll *colls = (th_coll *)p[5].ptrVal;

	th_ent_getcoll(e, scene, count, collC, maxColls, colls);
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
	th_sound_load((char *)p->ptrVal);

	r->ptrVal = thg.sound_count;
}

// sets array of sounds to be played
void umsoundloop(UmkaStackSlot *p, UmkaStackSlot *r) {
	if (!p[1].intVal || p[1].intVal-1 >= thg.sound_count) {
		th_error("Invalid sound\n");
		return;
	}
	th_sound *s = thg.sounds[p[1].intVal - 1];
	s->looping = p[0].intVal;
}

void umsoundplay(UmkaStackSlot *p, UmkaStackSlot *r) {
	if (!p[0].intVal || p[0].intVal-1 >= thg.sound_count) {
		th_error("Invalid sound\n");
		return;
	}
	th_sound *s = thg.sounds[p[0].intVal-1];
	s->playing = 1;
}

void umsoundstop(UmkaStackSlot *p, UmkaStackSlot *r) {
	if (!p[0].intVal || p[0].intVal-1 >= thg.sound_count) {
		th_error("Invalid sound\n");
		return;
	}
	th_sound *s = thg.sounds[p[0].intVal-1];
	s->playing = 0;
}

void umsoundvol(UmkaStackSlot *p, UmkaStackSlot *r) {
	if (!p[1].intVal || p[1].intVal-1 >= thg.sound_count) {
		th_error("Invalid sound\n");
		return;
	}
	th_sound *s = thg.sounds[p[1].intVal - 1];
	s->volume = p[0].real32Val;
}

// checks, if sound is valid
void umsoundvalidate(UmkaStackSlot *p, UmkaStackSlot *r) {
	uu s = p->intVal;

	if (!s || s-1 >= thg.sound_count) {
		r[0].intVal = 0;
		return;
	}

	r[0].intVal = 1;
}

///////////////////////
// raycast
void umraygetcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_ent **scene = (th_ent **)p[0].ptrVal;
	th_ray *ra = (th_ray *)p[1].ptrVal;
	int count = p[2].intVal;
	th_vf2 *ic = (th_vf2 *)p[3].ptrVal;

	r->intVal = th_ray_getcoll(ra, scene, count, ic);
}

void umraygettmapcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_ray *ra = (th_ray *)p[2].ptrVal;
	th_tmap *t = (th_tmap *)p[1].ptrVal;
	th_vf2 *ic = (th_vf2 *)p[0].ptrVal;

	r->intVal = th_ray_to_tilemap(ra, t, ic);
}

///////////////////////
// misc
// draws a rectangle showing, where the camera is
void umvisualizecam(UmkaStackSlot *p, UmkaStackSlot *r) {
	int w = p[2].intVal;
	int h = p[1].intVal;
	int color = p[0].uintVal;

	CNFGColor((uint32_t)color);
	CNFGTackRectangle(0, 0, w * thg.scaling, h * thg.scaling);
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
	fu size = p[0].real32Val;
	uint32_t color = (uint32_t)p[1].uintVal;
	th_vf2 pos = *(th_vf2 *)&p[2];
	char *text = (char *)p[3].ptrVal;

	CNFGPenX = pos.x * thg.scaling;
	CNFGPenY = pos.y * thg.scaling;
	CNFGColor(color);
	CNFGSetLineWidth(0.6 * thg.scaling * size);
	CNFGDrawText(text, size * thg.scaling);
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
	r->intVal = CNFGHandleInput();
}

// calculates scaling
void umgetscaling(UmkaStackSlot *p, UmkaStackSlot *r) {
	int camh = p[0].intVal;
	int camw = p[1].intVal;
	int h = p[2].intVal;
	int w = p[3].intVal;

	if ((float)w/camw < (float)h/camh) {
		thg.scaling = ((float)w/camw);
	} else {
		thg.scaling = ((float)h/camh);
	}
}

void umCNFGTackRectangle(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_rect re = *(th_rect *)&p[0];

	CNFGTackRectangle(re.x * thg.scaling, re.y * thg.scaling, (re.x + re.w) * thg.scaling, (re.y + re.h) * thg.scaling);
}

void umCNFGChangeWindowTitle(UmkaStackSlot *p, UmkaStackSlot *r) {
	CNFGChangeWindowTitle((char *)p->ptrVal);
}

void umCNFGSetWindowIconData(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_error("setasicon is deprecated.");
}

void umCNFGTackSegment(UmkaStackSlot *p, UmkaStackSlot *r) {
	float thickness = p[0].real32Val;
	th_vf2 e = *(th_vf2 *)&p[1];
	th_vf2 b = *(th_vf2 *)&p[2];


	CNFGSetLineWidth(thickness * thg.scaling);
	CNFGTackSegment(b.x * thg.scaling, b.y * thg.scaling, e.x * thg.scaling, e.y * thg.scaling);
}

void umCNFGBlitTex(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img;
	GET_IMAGE(img, p[1].ptrVal);
	th_transform *t = (th_transform *)p[0].ptrVal;

	th_blit_tex(img, *t);
}
