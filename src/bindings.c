#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#include <umka_api.h>
#include <stb_image.h>

#include "tophat.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

extern th_global *thg;

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
		strcpy(out, thg->respath);
		strcat(out, path);
	}
	return out;
}

void umth_fopen(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *name = (char *)p[1].ptrVal;
	const char *mode = (const char *)p[0].ptrVal;

	char path[512];
	FILE *f = fopen(conv_path(path, name), mode);
	r->ptrVal = f;
}

void umth_th_getglobal(UmkaStackSlot *p, UmkaStackSlot *r) {
	r->ptrVal = thg;
}

void umth_th_getfuncs(UmkaStackSlot *p, UmkaStackSlot *r) {
	int count = 0;
#define THEXT(ret, name, ...) ++count
#include "thextdata.h"
#undef THEXT

	void **arr = malloc(sizeof(void *) * count);

	int idx = 0;
#define THEXT(ret, name, ...) arr[idx++] = &name
#include "thextdata.h"
#undef THEXT
	r->ptrVal = arr;
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

// fn umth_font_load((2) path: str, (1) size: real, (0) filter: uint32) 
static void umth_font_load(UmkaStackSlot *p, UmkaStackSlot *r) {
	uint32_t filter = p[0].uintVal;
	double size = p[1].real32Val;
	char path[1024];
	conv_path(path, p[2].ptrVal);

	// NOTE(skejeton): The font load function may return null pointer on fail,
	//	   				     it would be the responsibility of the user to verify the validity.
	r->ptrVal = th_font_load(path, size, filter);
}

// fn umth_font_draw((5) font: Font, (4) s: str, (3) x: real, (2) y: real, (1) color: uint32, (0) scale: real)
static void umth_font_draw(UmkaStackSlot *p, UmkaStackSlot *r) {
	double scale = p[0].real32Val;
	uint32_t color = p[1].uintVal;
	double y = p[2].real32Val;
	double x = p[3].real32Val;
	const char *s = p[4].ptrVal;
	th_font *font = p[5].ptrVal;

	th_font_draw(font, s, x, y, color, scale);
}

// fn umth_font_measure((2) font: Font, (1) s: str, (0) o: ^th.Vf2)
static void umth_font_measure(UmkaStackSlot *p, UmkaStackSlot *r) {
	const char *s = p[1].ptrVal;
	th_font *font = p[2].ptrVal;

	*((th_vf2*)p[0].ptrVal) = th_font_measure(font, s);
}

///////////////////////
// particles
void umth_particles_draw(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_particles *emitter = (th_particles *)p[2].ptrVal;
	th_rect *cam = (th_rect *)p[1].ptrVal;
	int t = p[0].intVal;

	th_particles_draw(emitter, *cam, t);
}

///////////////////////
// tilemaps
// draws a tilemap takes a rectangle as a camera and the tilemap itself
void umth_tilemap_draw(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_rect *cam = (th_rect *)p[0].ptrVal;
	th_tmap *t = (th_tmap *)p[1].ptrVal;
	th_tmap_draw(t, cam);
}

// checks, if tilemap collides with entity.
// ent - entity to collide with, t - tilemap, x and y - pointers to ints used to return, where the collision occured
void umth_tilemap_getcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_ent *ent = (th_ent *)p[0].ptrVal;
	th_tmap *t = (th_tmap *)p[1].ptrVal;
	uu *vert = (uu *)p[2].ptrVal;
	th_vf2 *tc = (th_vf2 *)p[3].ptrVal;

	r->intVal = _th_coll_on_tilemap(ent, t, vert, tc);
}

void umth_tilemap_autotile(UmkaStackSlot *p, UmkaStackSlot *r) {
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
void umth_image_load(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image **img = p[1].ptrVal;
	char *path = (char *)p[0].ptrVal;

	char pathcpy[1024];
	*img = th_load_image(conv_path(pathcpy, path));
}

// flips image
void umth_image_flipv(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[1].ptrVal;
	if (!img) return;

	img->flipv = p[0].intVal;
}

// flips image
void umth_image_fliph(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[1].ptrVal;
	if (!img) return;

	img->fliph = p[0].intVal;
}

void umth_image_get_dims(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[0].ptrVal;
	if (!img) return;
	th_vf2 *out = (th_vf2 *)p[1].ptrVal;

	if (!img)
		return;

	*out = img->dm;
}

void umth_image_crop(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[2].ptrVal;
	if (!img) return;
	th_vf2 tl = *(th_vf2 *)&p[1];
	th_vf2 br = *(th_vf2 *)&p[0];

	th_image_crop(img, tl, br);
}

void umth_image_crop_quad(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[1].ptrVal;
	if (!img) return;
	th_quad *q = p[0].ptrVal;

	img->crop = *q;
}

void umth_image_get_crop_quad(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[1].ptrVal;
	if (!img) return;
	th_quad *q = p[0].ptrVal;

	*q = img->crop;
}

// returns a pointer to an image from data
void umth_image_from_data(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 dm = *(th_vf2 *)&p[0];
	uint32_t *data = (uint32_t *)p[1].ptrVal;
	th_image **ret = p[2].ptrVal;

	th_image *img = th_image_alloc();
	if (!img) return;
	th_image_from_data(img, data, dm);

	*ret = img;
}

void umth_image_copy(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img1 = p[0].ptrVal;
	th_image **ret = p[1].ptrVal;
	if (!img1) return;

	th_image *img2 = th_image_alloc();
	if (!img2) return;
	
	uint32_t *data = th_image_get_data(img1);

	th_image_from_data(img2, data, img1->dm);
	img2->flipv = img1->flipv;
	img2->fliph = img1->fliph;
	img2->crop = img1->crop;

	*ret = img2;

	free(data);
}

void umth_image_set_filter(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[1].ptrVal;
	if (!img) return;
	int filter = p[0].intVal;

	th_image_set_filter(img, filter);
}

void umth_image_update_data(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[2].ptrVal;
	if (!img) return;
	uint32_t *data = p[1].ptrVal;
	th_vf2 dm = *(th_vf2 *)&p[0];

	th_image_update_data(img, data, dm);
}

void umth_image_get_data(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[1].ptrVal;
	if (!img) return;
	uint32_t *o = p[0].ptrVal;

	uint32_t *data = th_image_get_data(img);
	memcpy(o, data, img->dm.w * img->dm.h * sizeof(uint32_t));

	free(data);
}

void umth_image_make_render_target(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[0].ptrVal;
	if (!img) return;

	th_image_set_as_render_target(img);
}

void umth_image_remove_render_target(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[1].ptrVal;
	th_vf2 wp = *(th_vf2 *)&p[0];

	th_image_remove_render_target(img, wp);
}

void umth_image_draw(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image_render_transformed(
		(th_image*)      p[2].ptrVal,
		*(th_transform*) p[1].ptrVal,
		(uint32_t) 			 p[0].uintVal
	);
}

void umth_image_draw_on_quad(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_quad q;
	q = *(th_quad *)&p[0];
	uint32_t filter = p[4].uintVal;
	th_image *img = p[5].ptrVal;	

	th_blit_tex(img, q, filter);
}

void umth_image_draw_nine_patch(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = p[4].ptrVal;
	if (!img) return;
	
	th_rect
		outer = *(th_rect*)p[3].ptrVal,
		inner = *(th_rect*)p[2].ptrVal,
		dest  = *(th_rect*)p[1].ptrVal;

	uint32_t tint = p[0].uintVal;

	if (dest.w < 0) {
    dest.x += dest.w;
    dest.w *= -1;
  }

  if (dest.h < 0) {
    dest.y += dest.h;
    dest.h *= -1;
  }

  th_vf2 stepSrc[3] = {{{inner.x, inner.y}}, {{inner.w, inner.h}}, {{outer.w-(inner.x+inner.w), outer.h-(inner.y+inner.h)}}};
  th_vf2 stepDst[3] = {{{stepSrc[0].x, stepSrc[0].y}}, {{dest.w-stepSrc[0].x-stepSrc[2].x, dest.h-stepSrc[0].y-stepSrc[2].y}}, {{stepSrc[2].x, stepSrc[2].y}}};

  th_vf2 src = {{outer.x, outer.y}};
  th_vf2 dst = {{dest.x, dest.y}};

  th_vf2 imgDims = img->dm;

  // failsafe
  if (imgDims.x == 0 || imgDims.y == 0) {
    return;
  }

  for (int x = 0; x < 3; x++) {
    float ssX = stepSrc[x].x;
    float sdX = stepDst[x].x;
    for (int y = 0; y < 3; y++) {
      float ssY = stepSrc[y].y;
      float sdY = stepDst[y].y;

      th_image_crop(img, (th_vf2){{src.x/imgDims.x, src.y/imgDims.y}}, (th_vf2){{(src.x+ssX)/imgDims.x, (src.y+ssY)/imgDims.y}});
      th_image_render_transformed(img, (th_transform){.scale = {{sdX/ssX, sdY/ssY}}, .pos = {{dst.x, dst.y}}}, tint);

      src.y += ssY;
      dst.y += sdY;
    }
    src.x += ssX;
    dst.x += sdX;
    src.y = outer.y;
    dst.y = dest.y;
  }
}

///////////////////////
// input
// gets position of mouse cursor
void umth_input_get_mouse(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 *out = (th_vf2 *)p[0].ptrVal;
	out->x = (thg->mouse.x - thg->offset.x) / thg->scaling;
	out->y = (thg->mouse.y - thg->offset.y) / thg->scaling;
}

void umth_input_is_pressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r->intVal = thg->pressed[keycode];
}

void umth_input_is_just_pressed(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r->intVal = thg->just_pressed[keycode];
}

void umth_input_is_just_released(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	r->intVal = thg->just_released[keycode];
}

void umth_input_clear(UmkaStackSlot *p, UmkaStackSlot *r) {
	int keycode = p[0].intVal;

	thg->just_pressed[keycode] = 0;
	thg->just_released[keycode] = 0;
	thg->pressed[keycode] = 0;
}

void umth_input_get_str(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *buf = umkaAllocData(thg->umka, thg->input_string_len + 1, NULL);
	buf[thg->input_string_len] = 0;
	memcpy(buf, thg->input_string, thg->input_string_len);

	r->ptrVal = buf;
}

void umth_input_get_mouse_delta(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 *o = (th_vf2 *)p[0].ptrVal;
	o->x = thg->mouse_delta.x / thg->scaling;
	o->y = thg->mouse_delta.y / thg->scaling;
}

void umth_input_get_mouse_scroll(UmkaStackSlot *p, UmkaStackSlot *r) {
	*(fu*)(p[0].ptrVal) = thg->mouse_wheel.y;
	*(fu*)(p[1].ptrVal) = thg->mouse_wheel.x;
}

///////////////////////
// entities
// draws an entity
void umth_ent_draw(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_rect *rc = (th_rect *)p[0].ptrVal;
	th_ent *e = (th_ent *)p[1].ptrVal;
	th_ent_draw(e, rc);
}

void umth_ent_getcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_ent **scene = (th_ent **)p[0].ptrVal;
	th_ent *e = (th_ent *)p[1].ptrVal;
	int count = p[2].intVal;
	uu maxColls = p[3].intVal;
	uu *collC = (uu *)p[4].ptrVal;
	th_coll *colls = (th_coll *)p[5].ptrVal;

	th_ent_getcoll(e, scene, count, collC, maxColls, colls);
}

static
int _th_ysort_test(const void *a, const void *b) {
	return ((th_ent *)a)->t.pos.y - ((th_ent *)b)->t.pos.y;
}

void umth_ent_ysort(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_ent *ents = (th_ent *)p[1].ptrVal;
	int count = p[0].intVal;

	qsort(ents, count, sizeof(th_ent), _th_ysort_test);
}

///////////////////////
// audio
void umth_sound_load(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *path = (char *)p[0].ptrVal;

	r->ptrVal = th_audio_load(path);
}

void umth_sound_copy(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[0].ptrVal;

	r->ptrVal = th_sound_copy(s);
}

void umth_sound_is_playing(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[0].ptrVal;

	r->intVal = ma_sound_is_playing(&s->inst);
}

void umth_sound_play(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[0].ptrVal;

	ma_sound_start(&s->inst);
}

void umth_sound_set_volume(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[1].ptrVal;
	float vol = p[0].real32Val;

	ma_sound_set_volume(&s->inst, vol);
}

void umth_sound_set_pan(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[1].ptrVal;
	float pan = p[0].real32Val;

	ma_sound_set_pan(&s->inst, pan);
}

void umth_sound_set_pitch(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[1].ptrVal;
	float pitch = p[0].real32Val;

	ma_sound_set_pitch(&s->inst, pitch);
}

void umth_sound_set_looping(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[1].ptrVal;
	int looping = p[0].intVal;

	ma_sound_set_looping(&s->inst, looping);
}

void umth_sound_stop(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[0].ptrVal;

	ma_sound_stop(&s->inst);
}

void umth_sound_seek_to_frame(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[1].ptrVal;
	int frame = p[0].intVal;

	ma_sound_seek_to_pcm_frame(&s->inst, frame);
}

void umth_sound_frame_count(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[0].ptrVal;

	ma_sound_get_length_in_pcm_frames(&s->inst, (ma_uint64 *)&r->uintVal);
}

void umth_sound_set_start_time_ms(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[1].ptrVal;
	uint64_t t = p[0].uintVal;

	ma_sound_set_start_time_in_milliseconds(&s->inst, t);
}

void umth_sound_set_stop_time_ms(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = p[1].ptrVal;
	uint64_t t = p[0].uintVal;

	ma_sound_set_stop_time_in_milliseconds(&s->inst, t);
}

///////////////////////
// raycast
void umth_ray_getcoll(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_coll *colls = p[5].ptrVal;
	int *count = p[4].ptrVal;
	int maxColls = p[3].intVal;
	int sceneLen = p[2].intVal;
	th_ray *ra = (th_ray *)p[1].ptrVal;
	th_ent **scene = (th_ent **)p[0].ptrVal;

	th_ray_getcoll(ra, colls, maxColls, count, scene, sceneLen);
}

void umth_ray_get_tilemap_coll(UmkaStackSlot *p, UmkaStackSlot *r) {
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

void umth_window_get_fullscreen(UmkaStackSlot *p, UmkaStackSlot *r) {
	r->uintVal = th_window_is_fullscreen();
}

void umth_window_set_fullscreen(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_window_set_fullscreen(p->uintVal);
}

void umth_window_setup(UmkaStackSlot *p, UmkaStackSlot *r) {
	char *title = (char *)p[2].ptrVal;
	int w = p[1].intVal;
	int h = p[0].intVal;

	th_window_setup(title, w, h);

	th_image_init();
	th_canvas_init();
}

// draws text
void umdrawtext(UmkaStackSlot *p, UmkaStackSlot *r) {
	fu size = p[0].real32Val;
	uint32_t color = (uint32_t)p[1].uintVal;
	th_vf2 pos = *(th_vf2 *)&p[2];
	char *text = (char *)p[3].ptrVal;

	th_canvas_text(text, color, pos, size);
}

void umth_window_get_dimensions(UmkaStackSlot *p, UmkaStackSlot *r) {
	int *w = (int *)p[1].ptrVal;
	int *h = (int *)p[0].ptrVal;

	th_window_get_dimensions(w, h);
}

#ifdef _WIN32
static double time_now() {
	LARGE_INTEGER cnt, frq;
	QueryPerformanceCounter(&cnt);
	QueryPerformanceFrequency(&frq);

	return (double)cnt.QuadPart/(double)frq.QuadPart;
}
#endif

void umth_window_sleep(UmkaStackSlot *p, UmkaStackSlot *r) {
	int ms = p[0].intVal;

#ifdef _WIN32
  double sec = ms/1000.0;
  double time_start = time_now();
  double time = time_start;
  while ((time - time_start) < sec) {
    Sleep(0);
    time = time_now();
  }
#else
	usleep(ms * 1000);
#endif
}

// fn umth_window_set_viewport(dm: th.Vf2)
void umth_window_set_viewport(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 dm = *(th_vf2 *)&p[0];
	th_calculate_scaling(dm.w, dm.h);
}

void umth_window_set_dims(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 dm = *(th_vf2 *)&p[0];
	th_window_set_dims(dm);
}

void umth_window_set_icon(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_image *img = (th_image *)p[0].ptrVal;

	th_window_set_icon(img);
}

void umth_window_show_cursor(UmkaStackSlot *p, UmkaStackSlot *r) {
	bool show = p[0].intVal;
	th_window_show_cursor(show);
}

void umth_window_freeze_cursor(UmkaStackSlot *p, UmkaStackSlot *r) {
	bool freeze = p[0].intVal;
	th_window_freeze_cursor(freeze);
}

extern int *th_sapp_swap_interval;
void umth_window_set_target_fps(UmkaStackSlot *p, UmkaStackSlot *r) {
	int fps = p[0].intVal;
	if (fps < 0) {
		fps = 0;
	}
	*th_sapp_swap_interval = fps;
}

// draws text
void umth_canvas_draw_text(UmkaStackSlot *p, UmkaStackSlot *r) {
	fu size = p[0].real32Val;
	uint32_t color = (uint32_t)p[1].uintVal;
	th_vf2 pos = *(th_vf2 *)&p[2];
	char *text = (char *)p[3].ptrVal;

	th_canvas_text(text, color, pos, size);
}

void umth_canvas_draw_rect(UmkaStackSlot *p, UmkaStackSlot *r) {
	uint32_t color = p[2].uintVal;
	th_rect re = *(th_rect *)&p[0];
	th_canvas_rect(color, re);
}

void umth_canvas_draw_line(UmkaStackSlot *p, UmkaStackSlot *r) {
	float thickness = p[0].real32Val;
	th_vf2 e = *(th_vf2 *)&p[1];
	th_vf2 b = *(th_vf2 *)&p[2];
	uint32_t color = p[3].uintVal;

	th_canvas_line(color, b, e, thickness);
}

void umth_canvas_draw_quad(UmkaStackSlot *p, UmkaStackSlot *r) {
	uint32_t color = p[1].uintVal;
	th_quad *q = p[0].ptrVal;

	th_canvas_quad(q, color);
}

void umth_utf8_get_next_rune(UmkaStackSlot *p, UmkaStackSlot *r) {
	int idx = p[0].intVal;
	char *str = p[1].ptrVal;

	uint32_t rune;
	th_utf8_decode(&rune, &str[idx]);
	r->uintVal = rune;
}

void umth_transform_rect(UmkaStackSlot *p, UmkaStackSlot *_) {
	th_quad *ret = p[2].ptrVal;
	th_rect *r = p[1].ptrVal;
	th_transform *t = p[0].ptrVal;

	th_transform_rect(ret, *t, *r);
}

void umth_transform_quad(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_quad *q = p[1].ptrVal;
	th_transform *t = p[0].ptrVal;

	th_transform_quad(q, *t);
}

void umth_transform_vf2(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 *v = p[1].ptrVal;
	th_transform *t = p[0].ptrVal;

	th_transform_vf2(v, *t);
}

void umth_transform_transform(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_transform *o = (th_transform *)p[1].ptrVal;
	th_transform *t = p[0].ptrVal;
	th_transform_transform(o, *t);
}

void umth_coll_line_to_line(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 *b1 = p[4].ptrVal;
	th_vf2 *e1 = p[3].ptrVal;
	th_vf2 *b2 = p[2].ptrVal;
	th_vf2 *e2 = p[1].ptrVal;
	th_vf2 *ic = p[0].ptrVal;

	r->intVal = th_line_to_line(*b1, *e1, *b2, *e2, ic);
}

void umth_coll_point_to_quad(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 *v = p[2].ptrVal;
	th_quad *q = p[1].ptrVal;
	th_vf2 *ic = p[0].ptrVal;

	r->intVal = th_point_to_quad(*v, q, ic);
}

void umth_coll_line_to_quad(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 *b = p[3].ptrVal;
	th_vf2 *e = p[2].ptrVal;
	th_quad *q = p[1].ptrVal;
	th_vf2 *ic = p[0].ptrVal;

	r->intVal = th_line_to_quad(*b, *e, q, ic);
}

void umth_coll_quad_to_quad(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_quad *q1 = p[2].ptrVal;
	th_quad *q2 = p[1].ptrVal;
	th_vf2 *ic = p[0].ptrVal;

	r->intVal = th_quad_to_quad(q1, q2, ic);
}

void umth_coll_point_to_rect(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 v = *(th_vf2 *)&p[1];
	th_rect *re = p[0].ptrVal;

	r->intVal = th_coll_point_on_rect(v, re);
}

void umth_coll_rect_to_rect(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_rect *r1 = p[1].ptrVal;
	th_rect *r2 = p[0].ptrVal;

	r->intVal = th_rect_to_rect(r1, r2);
}

void umth_nav_mesh_add_quad(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_navmesh *m = p[1].ptrVal;
	th_quad *q = p[0].ptrVal;

	th_navmesh_add_quad(m, q);
}

void umth_nav_mesh_nav(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_vf2 *cameFrom = p[3].ptrVal;
	th_navmesh *m = p[2].ptrVal;
	th_vf2 p1 = *(th_vf2 *)&p[1];
	th_vf2 p2 = *(th_vf2 *)&p[0];

	th_navmesh_nav(cameFrom, m, p1, p2);
}

void _th_umka_bind(void *umka) {
	// etc
	umkaAddFunc(umka, "umth_fopen", &umth_fopen);
	umkaAddFunc(umka, "umth_th_getglobal", &umth_th_getglobal);
	umkaAddFunc(umka, "umth_th_getfuncs", &umth_th_getfuncs);

	// color
	umkaAddFunc(umka, "umth_hsv2rgb_uint32", &umth_hsv2rgb_uint32);
	umkaAddFunc(umka, "umth_rgb_uint32", &umth_rgb_uint32);

	// font
	umkaAddFunc(umka, "umth_font_load", &umth_font_load);
	umkaAddFunc(umka, "umth_font_draw", &umth_font_draw);
	umkaAddFunc(umka, "umth_font_measure", &umth_font_measure);

	// particles
	umkaAddFunc(umka, "umth_particles_draw", &umth_particles_draw);

	// tilemaps
	umkaAddFunc(umka, "umth_tilemap_draw", &umth_tilemap_draw);
	umkaAddFunc(umka, "umth_tilemap_getcoll", &umth_tilemap_getcoll);
	umkaAddFunc(umka, "umth_tilemap_autotile", &umth_tilemap_autotile);

	// images
	umkaAddFunc(umka, "umth_image_load", &umth_image_load);
	umkaAddFunc(umka, "umth_image_flipv", &umth_image_flipv);
	umkaAddFunc(umka, "umth_image_fliph", &umth_image_fliph);
	umkaAddFunc(umka, "umth_image_get_dims", &umth_image_get_dims);
	umkaAddFunc(umka, "umth_image_crop", &umth_image_crop);
	umkaAddFunc(umka, "umth_image_crop_quad", &umth_image_crop_quad);
	umkaAddFunc(umka, "umth_image_get_crop_quad", &umth_image_get_crop_quad);
	umkaAddFunc(umka, "umth_image_from_data", &umth_image_from_data);
	umkaAddFunc(umka, "umth_image_copy", &umth_image_copy);
	umkaAddFunc(umka, "umth_image_set_filter", &umth_image_set_filter);
	umkaAddFunc(umka, "umth_image_update_data", &umth_image_update_data);
	umkaAddFunc(umka, "umth_image_get_data", &umth_image_get_data);
	umkaAddFunc(umka, "umth_image_make_render_target",
		&umth_image_make_render_target);
	umkaAddFunc(umka, "umth_image_remove_render_target",
		&umth_image_remove_render_target);
	umkaAddFunc(umka, "umth_image_draw", &umth_image_draw);
	umkaAddFunc(umka, "umth_image_draw_on_quad", &umth_image_draw_on_quad);
	umkaAddFunc(umka, "umth_image_draw_nine_patch", &umth_image_draw_nine_patch);

	// input
	umkaAddFunc(umka, "umth_input_get_mouse", &umth_input_get_mouse);
	umkaAddFunc(umka, "umth_input_is_pressed", &umth_input_is_pressed);
	umkaAddFunc(umka, "umth_input_is_just_pressed", &umth_input_is_just_pressed);
	umkaAddFunc(umka, "umth_input_is_just_released", &umth_input_is_just_released);
	umkaAddFunc(umka, "umth_input_clear", &umth_input_clear);
	umkaAddFunc(umka, "umth_input_get_str", &umth_input_get_str);
	umkaAddFunc(umka, "umth_input_get_mouse_delta", &umth_input_get_mouse_delta);
	umkaAddFunc(umka, "umth_input_get_mouse_scroll", &umth_input_get_mouse_scroll);

	// entities
	umkaAddFunc(umka, "umth_ent_draw", &umth_ent_draw);
	umkaAddFunc(umka, "umth_ent_getcoll", &umth_ent_getcoll);
	umkaAddFunc(umka, "umth_ent_ysort", &umth_ent_ysort);

	// rays
	umkaAddFunc(umka, "umth_ray_getcoll", &umth_ray_getcoll);
	umkaAddFunc(umka, "umth_ray_get_tilemap_coll", &umth_ray_get_tilemap_coll);

	// audio
	umkaAddFunc(umka, "umth_sound_load", &umth_sound_load);
	umkaAddFunc(umka, "umth_sound_copy", &umth_sound_copy);
	umkaAddFunc(umka, "umth_sound_is_playing", &umth_sound_is_playing);
	umkaAddFunc(umka, "umth_sound_play", &umth_sound_play);
	umkaAddFunc(umka, "umth_sound_set_volume", &umth_sound_set_volume);
	umkaAddFunc(umka, "umth_sound_set_pan", &umth_sound_set_pan);
	umkaAddFunc(umka, "umth_sound_set_pitch", &umth_sound_set_pitch);
	umkaAddFunc(umka, "umth_sound_set_looping", &umth_sound_set_looping);
	umkaAddFunc(umka, "umth_sound_stop", &umth_sound_stop);
	umkaAddFunc(umka, "umth_sound_seek_to_frame", &umth_sound_seek_to_frame);
	umkaAddFunc(umka, "umth_sound_frame_count", &umth_sound_frame_count);
	umkaAddFunc(umka, "umth_sound_set_start_time_ms",
		&umth_sound_set_start_time_ms);
	umkaAddFunc(umka, "umth_sound_set_stop_time_ms",
		&umth_sound_set_stop_time_ms);

	// window
	umkaAddFunc(umka, "umth_window_begin_scissor", &umth_window_begin_scissor);
	umkaAddFunc(umka, "umth_window_end_scissor", &umth_window_end_scissor);
	umkaAddFunc(umka, "umth_window_setup", &umth_window_setup);
	umkaAddFunc(umka, "umth_window_get_dimensions", &umth_window_get_dimensions);
	umkaAddFunc(umka, "umth_window_sleep", &umth_window_sleep);

	// window
	umkaAddFunc(umka, "umth_window_get_fullscreen", &umth_window_get_fullscreen);
	umkaAddFunc(umka, "umth_window_set_fullscreen", &umth_window_set_fullscreen);
	umkaAddFunc(umka, "umth_window_begin_scissor", &umth_window_begin_scissor);
	umkaAddFunc(umka, "umth_window_end_scissor", &umth_window_end_scissor);
	umkaAddFunc(umka, "umth_window_setup", &umth_window_setup);
	umkaAddFunc(umka, "umth_window_get_dimensions", &umth_window_get_dimensions);
	umkaAddFunc(umka, "umth_window_sleep", &umth_window_sleep);
	umkaAddFunc(umka, "umth_window_set_viewport", &umth_window_set_viewport);
	umkaAddFunc(umka, "umth_window_set_dims", &umth_window_set_dims);
	umkaAddFunc(umka, "umth_window_set_icon", &umth_window_set_icon);
	umkaAddFunc(umka, "umth_window_show_cursor", &umth_window_show_cursor);
	umkaAddFunc(umka, "umth_window_freeze_cursor", &umth_window_freeze_cursor);
	umkaAddFunc(umka, "umth_window_set_target_fps", &umth_window_set_target_fps);

	// canvas
	umkaAddFunc(umka, "umth_canvas_draw_text", &umth_canvas_draw_text);
	umkaAddFunc(umka, "umth_canvas_draw_rect", &umth_canvas_draw_rect);
	umkaAddFunc(umka, "umth_canvas_draw_line", &umth_canvas_draw_line);
	umkaAddFunc(umka, "umth_canvas_draw_quad", &umth_canvas_draw_quad);

	// utf8
	umkaAddFunc(umka, "umth_utf8_get_next_rune", &umth_utf8_get_next_rune);

	// transform
	umkaAddFunc(umka, "umth_transform_rect", &umth_transform_rect);
	umkaAddFunc(umka, "umth_transform_quad", &umth_transform_quad);
	umkaAddFunc(umka, "umth_transform_vf2", &umth_transform_vf2);
	umkaAddFunc(umka, "umth_transform_transform", &umth_transform_transform);

	// colisions
	umkaAddFunc(umka, "umth_coll_line_to_line", &umth_coll_line_to_line);
	umkaAddFunc(umka, "umth_coll_point_to_quad", &umth_coll_point_to_quad);
	umkaAddFunc(umka, "umth_coll_line_to_quad", &umth_coll_line_to_quad);
	umkaAddFunc(umka, "umth_coll_quad_to_quad", &umth_coll_quad_to_quad);
	umkaAddFunc(umka, "umth_coll_point_to_rect", &umth_coll_point_to_rect);
	umkaAddFunc(umka, "umth_coll_rect_to_rect", &umth_coll_rect_to_rect);

	// nav
	umkaAddFunc(umka, "umth_nav_mesh_add_quad", &umth_nav_mesh_add_quad);
	umkaAddFunc(umka, "umth_nav_mesh_nav", &umth_nav_mesh_nav);

	for (int i = 0; i < th_em_modulenames_count; i++) {
		umkaAddModule(umka, th_em_modulenames[i], th_em_modulesrc[i]);
	}
}
