#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

#include <chew.h>
#include <umka_api.h>
#include <stb_image.h>

#include "tophat.h"

extern th_global thg;

extern char *th_em_modulenames[];
extern char *th_em_modulesrc[];
extern char *th_em_misc[];
extern int th_em_modulenames_count;

// FIXME(skejeton): Using this function is an easy buffer overflow.
static
char *conv_path(char *out, char *path) {
	const char *RAW_PFX = "raw://";
	const int RAW_PFX_LEN = strlen("raw://");

	if (strncmp(path, RAW_PFX, RAW_PFX_LEN) == 0) {
		strcpy(out, path + RAW_PFX_LEN);
	} else {
		strcpy(out, thg.respath);
		strcat(out, path);
	}
	return out;
}

void umfopen(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *name = (char *)p[1].ptrVal;
	const char *mode = (const char *)p[0].ptrVal;

	char path[512];
	FILE *f = fopen(conv_path(path, name), mode);
	r->ptrVal = f;
}

///////////////////////////
// COLOUR

// fn umth_hsv2rgb_uint32((3) h, (2) s, (1) v, (0) a: th.fu): uint32
static void umth_hsv2rgb_uint32(UmkaStackSlot *p, UmkaStackSlot *r) {
	r->uintVal = th_color_hsv2rgb(p[3].real32Val, p[2].real32Val, p[1].real32Val, p[0].real32Val);
}

// fn umth_rgb_uint32((3) r, (2) g, (1) b, (0) a: th.fu): uint32
static void umth_rgb_uint32(UmkaStackSlot *p, UmkaStackSlot *r) {
	r->uintVal = th_color_rgb(p[3].real32Val, p[2].real32Val, p[1].real32Val, p[0].real32Val);
}

///////////////////////////
// FONT

// fn cfontload((2) path: str, (1) size: real, (0) filter: uint32) 
static void umfontload(UmkaStackSlot *p, UmkaStackSlot *r) {
	uint32_t filter = p[0].uintVal;
	double size = p[1].real32Val;
	char path[1024];
	conv_path(path, p[2].ptrVal);

	// NOTE(skejeton): The font load function may return null pointer on fail,
	//	   				     it would be the responsibility of the user to verify the validity.
	r->ptrVal = th_font_load(path, size, filter);
}

// fn cfontdraw((5) font: Font, (4) s: str, (3) x: real, (2) y: real, (1) color: uint32, (0) scale: real)
static void umfontdraw(UmkaStackSlot *p, UmkaStackSlot *r) {
	double scale = p[0].real32Val;
	uint32_t color = p[1].uintVal;
	double y = p[2].real32Val;
	double x = p[3].real32Val;
	const char *s = p[4].ptrVal;
	th_font *font = p[5].ptrVal;

	th_font_draw(font, s, x, y, color, scale);
}

// fn cfontmeasure((2) font: Font, (1) s: str, (0) o: ^th.Vf2)
static void umfontmeasure(UmkaStackSlot *p, UmkaStackSlot *r) {
	const char *s = p[1].ptrVal;
	th_font *font = p[2].ptrVal;

	*((th_vf2*)p[0].ptrVal) = th_font_measure(font, s);
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

void umtmapautotile(UmkaStackSlot *p, UmkaStackSlot *r) {
	uu tile = p[0].intVal;
	uu *cfg = (uu *)p[1].ptrVal;
	uu *src = (uu *)p[2].ptrVal;
	uu h = p[3].intVal;
	uu w = p[4].intVal;
	uu *tgt = (uu *)p[5].ptrVal;

	th_tmap_autotile(tgt, src, w, h, cfg, tile);
}

///////////////////////
// images
// loads an image at respath + path
void umimgload(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image **img = p[1].ptrVal;
	char *path = (char *)p[0].ptrVal;

	char pathcpy[1024];
	*img = th_load_image(conv_path(pathcpy, path));
}

// flips image
void umimgflipv(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[1].ptrVal;
	if (!img) return;

	img->flipv = p[0].intVal;
}

// flips image
void umimgfliph(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[1].ptrVal;
	if (!img) return;

	img->fliph = p[0].intVal;
}

void umimggetdims(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[0].ptrVal;
	if (!img) return;
	th_vf2 *out = (th_vf2 *)p[1].ptrVal;

	if (!img)
		return;

	*out = img->dm;
}

void umimgcrop(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[2].ptrVal;
	if (!img) return;
	th_vf2 tl = *(th_vf2 *)&p[1];
	th_vf2 br = *(th_vf2 *)&p[0];

	img->crop = (th_rect){tl.x, tl.y, br.x, br.y};
}

// returns a pointer to an image from data
void umimgfromdata(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 dm = *(th_vf2 *)&p[0];
	uint32_t *data = (uint32_t *)p[1].ptrVal;
	th_image **ret = p[2].ptrVal;

	th_image *img = th_image_alloc();
	if (!img) return;
	th_image_from_data(img, data, dm);

	*ret = img;
}

void umimgcopy(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img1 = p[0].ptrVal;
	th_image **ret = p[1].ptrVal;
	if (!img1) return;

	th_image *img2 = th_image_alloc();
	if (!img2) return;
	th_image_from_data(img2, img1->data, img1->dm);
	img2->flipv = img1->flipv;
	img2->fliph = img1->fliph;
	img2->crop = img1->crop;

	*ret = img2;
}

void umimgsetfilter(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[1].ptrVal;
	if (!img) return;
	int filter = p[0].intVal;

	th_image_set_filter(img, filter);
}

void umimgupdatedata(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[2].ptrVal;
	if (!img) return;
	uint32_t *data = p[1].ptrVal;
	th_vf2 dm = *(th_vf2 *)&p[0];

	th_image_update_data(img, data, dm);
}

void umimggetdata(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[1].ptrVal;
	if (!img) return;
	uint32_t *data = p[0].ptrVal;
	memcpy(data, img->data, sizeof(uint32_t) * img->dm.w * img->dm.h);
}

///////////////////////
// input
// gets position of mouse cursor
void umgetmouse(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 *out = (th_vf2 *)p[0].ptrVal;
	out->x = (thg.mouse.x - thg.offset.x) / thg.scaling;
	out->y = (thg.mouse.y - thg.offset.y) / thg.scaling;
}

void umispressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r[0].intVal = thg.pressed[keycode];
}

void umisjustpressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r[0].intVal = thg.just_pressed[keycode];
}

void umclear(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	thg.just_pressed[keycode] = 0;
	thg.pressed[keycode] = 0;
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
	uu maxColls = p[3].intVal;
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
void th_playback_destructor(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_playback *pk = p[0].ptrVal;

	ma_decoder_uninit(pk->decoder);
	free(pk->decoder);
}

void umsoundplay(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[0].ptrVal;

	th_playback_item *pbi = malloc(sizeof(th_playback_item));

	pbi->next = thg.playbacks;
	thg.playbacks = pbi;

	pbi->pk = umkaAllocData(thg.umka, sizeof(th_playback), th_playback_destructor);
	umkaIncRef(thg.umka, pbi->pk);

	*pbi->pk = (th_playback){
		.decoder = malloc(sizeof(ma_decoder)),
		.playing = 1,
		.paused  = 0,
		.looping = s->looping,
		.volume  = s->volume,
		.frame   = -1
	};

	extern ma_decoder_config decodercfg;
	ma_result res;
	res = ma_decoder_init_file(s->path, &decodercfg, pbi->pk->decoder);
	if (res != MA_SUCCESS)
		th_error("Couldn't load sound at path %s", s->path);

	r->ptrVal = pbi->pk;
}

///////////////////////
// raycast
void umraygetcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_coll *colls = p[5].ptrVal;
	int *count = p[4].ptrVal;
	int maxColls = p[3].intVal;
	int sceneLen = p[2].intVal;
	th_ray *ra = (th_ray *)p[1].ptrVal;
	th_ent **scene = (th_ent **)p[0].ptrVal;

	th_ray_getcoll(ra, colls, maxColls, count, scene, sceneLen);
}

void umraygettmapcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_ray *ra = (th_ray *)p[2].ptrVal;
	th_tmap *t = (th_tmap *)p[1].ptrVal;
	th_vf2 *ic = (th_vf2 *)p[0].ptrVal;

	r->intVal = th_ray_to_tilemap(ra, t, ic);
}

///////////////////////
// misc

void umth_window_begin_scissor(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_window_begin_scissor(p[3].intVal, p[2].intVal, p[1].uintVal, p[0].uintVal);
}

void umth_window_end_scissor(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_window_end_scissor();
}

// draws text
void umdrawtext(UmkaStackSlot *p, UmkaStackSlot *r) {
	fu size = p[0].real32Val;
	uint32_t color = (uint32_t)p[1].uintVal;
	th_vf2 pos = *(th_vf2 *)&p[2];
	char *text = (char *)p[3].ptrVal;

	th_canvas_text(text, color, pos, size);
}

void umwindowsetup(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *title = (char *)p[2].ptrVal;
	int w = p[1].intVal;
	int h = p[0].intVal;

	th_window_setup(title, w, h);

	th_gl_init();
	th_image_init();
	th_canvas_init();
}

void umwindowclearframe(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_window_clear_frame();
}

void umwindowgetdimensions(UmkaStackSlot *p, UmkaStackSlot *r) {
	int *w = (int *)p[1].ptrVal;
	int *h = (int *)p[0].ptrVal;

	th_window_get_dimensions(w, h);
}

void umwindowswapbuffers(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_window_swap_buffers();
}

void umwindowhandle(UmkaStackSlot *p, UmkaStackSlot *r) {
	r->intVal = th_window_handle();
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

	thg.offset.x = (w - camw*thg.scaling)/2;
	thg.offset.y = (h - camh*thg.scaling)/2;
}

void umcanvasrect(UmkaStackSlot *p, UmkaStackSlot *r) {
	uint32_t color = p[2].uintVal;
	th_rect re = *(th_rect *)&p[0];
	th_canvas_rect(color, re);
}

void umcanvasline(UmkaStackSlot *p, UmkaStackSlot *r) {
	float thickness = p[0].real32Val;
	th_vf2 e = *(th_vf2 *)&p[1];
	th_vf2 b = *(th_vf2 *)&p[2];
	uint32_t color = p[3].uintVal;

	th_canvas_line(color, b, e, thickness);
}

void umimagedraw(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[2].ptrVal;
	if (!img) return;
	th_transform *t = (th_transform *)p[1].ptrVal;
	uint32_t c = p[0].uintVal;

	th_quad q = {0};
	th_transform_rect(&q, *t, (th_rect){
		.w = (img->crop.w - img->crop.x) * img->dm.x,
		.h = (img->crop.h - img->crop.y) * img->dm.y});
	th_blit_tex(img, q, c);
}

void umimgdrawonquad(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_quad q;
	q = *(th_quad *)&p[0];
	uint32_t filter = p[4].uintVal;
	th_image *img = p[5].ptrVal;	

	th_blit_tex(img, q, filter);
}

void umutf8getnextrune(UmkaStackSlot *p, UmkaStackSlot *r) {
	int idx = p[0].intVal;
	char *str = p[1].ptrVal;

	uint32_t rune;
	th_utf8_decode(&rune, &str[idx]);
	r->uintVal = rune;
}

void umcompilecanvasshader(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *frag = p[0].ptrVal;
	char *vert = p[1].ptrVal;

	r->intVal = th_canvas_compile_shader(vert, frag);
}

void umcompileimageshader(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *frag = p[0].ptrVal;
	char *vert = p[1].ptrVal;

	r->intVal = th_image_compile_shader(vert, frag);
}

void umpickcanvasshader(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_shader *s = th_get_shader_err(p[0].intVal);
	if (!s) return;

	th_canvas_flush();
	extern GLuint th_canvas_prog;
	th_canvas_prog = *s;
}

void umpickimageshader(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_shader *s = th_get_shader_err(p[0].intVal);
	if (!s) return;

	th_image_flush();
	extern GLuint th_blit_prog;
	th_blit_prog = *s;
}

void umgetuniformlocation(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_shader *s = th_get_shader_err(p[1].intVal);
	if (!s) return;

	glUseProgram(*s);
	r->intVal = glGetUniformLocation(*s, p[0].ptrVal);
}

void umsetuniformint(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_shader *s = th_get_shader_err(p[2].intVal);
	if (!s) return;

	th_canvas_flush();
	th_image_flush();
	glUseProgram(*s);
	glUniform1i(p[1].intVal, p[0].intVal);
}

void umsetuniformvf2(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_shader *s = th_get_shader_err(p[2].intVal);
	if (!s) return;
	th_vf2 *v = (th_vf2 *)&p[0];

	int sw, sh;
	th_window_get_dimensions(&sw, &sh);
	sw *= 0.5f;
	sh *= -0.5f;

	th_canvas_flush();
	th_image_flush();
	glUseProgram(*s);
	glUniform2f(p[1].intVal, v->x / sw + 1.f, v->y / sh + 1.f);
}

void _th_umka_bind(void *umka) {
	// etc
	umkaAddFunc(umka, "cfopen", &umfopen);

	// color
	umkaAddFunc(umka, "umth_hsv2rgb_uint32", &umth_hsv2rgb_uint32);
	umkaAddFunc(umka, "umth_rgb_uint32", &umth_rgb_uint32);

	// font
	umkaAddFunc(umka, "cfontload", &umfontload);
	umkaAddFunc(umka, "cfontdraw", &umfontdraw);
	umkaAddFunc(umka, "cfontmeasure", &umfontmeasure);

	// light
	umkaAddFunc(umka, "clightmaskclear", &umlightmaskclear);
	umkaAddFunc(umka, "clightmaskdraw", &umlightmaskdraw);
	umkaAddFunc(umka, "cspotlightstamp", &umspotlightstamp);

	// particles
	umkaAddFunc(umka, "c_particles_draw", &umparticlesdraw);

	// tilemaps
	umkaAddFunc(umka, "cdrawtmap", &umdrawtmap);
	umkaAddFunc(umka, "ctmapgetcoll", &umtmapgetcoll);
	umkaAddFunc(umka, "cautotile", &umtmapautotile);

	// images
	umkaAddFunc(umka, "loadimg", &umimgload);
	umkaAddFunc(umka, "flipvimg", &umimgflipv);
	umkaAddFunc(umka, "fliphimg", &umimgfliph);
	umkaAddFunc(umka, "imggetdims", &umimggetdims);
	umkaAddFunc(umka, "imgcrop", &umimgcrop);
	umkaAddFunc(umka, "imgfromdata", &umimgfromdata);
	umkaAddFunc(umka, "imgcopy", &umimgcopy);
	umkaAddFunc(umka, "imgsetfilter", &umimgsetfilter);
	umkaAddFunc(umka, "imgdrawonquad", &umimgdrawonquad);
	umkaAddFunc(umka, "imgupdatedata", &umimgupdatedata);
	umkaAddFunc(umka, "imggetdata", &umimggetdata);

	// input
	umkaAddFunc(umka, "cgetmouse", &umgetmouse);
	umkaAddFunc(umka, "cispressed", &umispressed);
	umkaAddFunc(umka, "cisjustpressed", &umisjustpressed);
	umkaAddFunc(umka, "cclear", &umclear);

	// entities
	umkaAddFunc(umka, "centdraw", &umentdraw);
	umkaAddFunc(umka, "cgetcoll", &umentgetcoll);
	umkaAddFunc(umka, "centysort", &umentysort);

	// rays
	umkaAddFunc(umka, "craygetcoll", &umraygetcoll);
	umkaAddFunc(umka, "craygettmapcoll", &umraygettmapcoll);

	// audio
	umkaAddFunc(umka, "csoundplay", &umsoundplay);

	// canvas
	umkaAddFunc(umka, "umth_window_begin_scissor", &umth_window_begin_scissor);
	umkaAddFunc(umka, "umth_window_end_scissor", &umth_window_end_scissor);
	umkaAddFunc(umka, "drawText", &umdrawtext);
	umkaAddFunc(umka, "wsetup", &umwindowsetup);
	umkaAddFunc(umka, "clearframe", &umwindowclearframe);
	umkaAddFunc(umka, "getdimensions", &umwindowgetdimensions);
	umkaAddFunc(umka, "swapbuffers", &umwindowswapbuffers);
	umkaAddFunc(umka, "handleinput", &umwindowhandle);
	umkaAddFunc(umka, "updatescaling", &umgetscaling);
	umkaAddFunc(umka, "drawRect", &umcanvasrect);
	umkaAddFunc(umka, "drawLine", &umcanvasline);
	umkaAddFunc(umka, "cdrawimage", &umimagedraw);

	// utf8
	umkaAddFunc(umka, "getNextRune", &umutf8getnextrune);

	// shader
	umkaAddFunc(umka, "csetuniformint", umsetuniformint);
	umkaAddFunc(umka, "csetuniformvf2", umsetuniformvf2);
	umkaAddFunc(umka, "ccompilecanvasshader", umcompilecanvasshader);
	umkaAddFunc(umka, "ccompileimageshader", umcompileimageshader);
	umkaAddFunc(umka, "cpickcanvasshader", umpickcanvasshader);
	umkaAddFunc(umka, "cpickimageshader", umpickimageshader);
	umkaAddFunc(umka, "cgetuniformlocation", umgetuniformlocation);

	int index = 0;
	for (int i = 0; i < th_em_modulenames_count; i++) {
		umkaAddModule(umka, th_em_modulenames[i], th_em_modulesrc[i]);
	}
}
