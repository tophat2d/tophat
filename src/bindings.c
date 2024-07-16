#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stb_image.h>
#include <umka_api.h>

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
static char *
conv_path(char *path)
{
	enum prefix
	{
		PFX_RAW,
		PFX_RES,
		PFX_DATA,
	};

	const char *prefixes[] = {
	    [PFX_RAW] = "raw://",
	    [PFX_RES] = "res://",
	    [PFX_DATA] = "data://",
	};

	const ssize_t path_len = strlen(path);
	enum prefix pfx = PFX_RAW;

	for (int i = 0; i < sizeof(prefixes) / sizeof(prefixes[0]); ++i) {
		const ssize_t prefix_len = strlen(prefixes[i]);
		if (path_len >= prefix_len && memcmp(path, prefixes[i], prefix_len) == 0) {
			pfx = i;
			path += prefix_len;
			break;
		}
	}

	char *out = NULL;

	switch (pfx) {
	case PFX_RAW: out = strdup(path); break;
	case PFX_RES:
		out = calloc(1, strlen(thg->res_dir) + strlen(path) + 1);
		strcpy(out, thg->res_dir);
		strcat(out, path);
		break;
	case PFX_DATA:
		out = calloc(1, strlen(thg->data_dir) + strlen(path) + 1);
		strcpy(out, thg->data_dir);
		strcat(out, path);
		break;
	}

	return out;
}

// fn umth_fopen(name: str, mode: str): std::File
void
umth_fopen(UmkaStackSlot *p, UmkaStackSlot *r)
{
	char *name = (char *)umkaGetParam(p, 0)->ptrVal;
	const char *mode = (const char *)umkaGetParam(p, 1)->ptrVal;

	printf("fopen\n");

	char *path = conv_path(name);
	FILE *f = fopen(path, mode);
	free(path);

	umkaGetResult(p, r)->ptrVal = f;
}

// fn umth_th_getglobal(): ^struct{}
void
umth_th_getglobal(UmkaStackSlot *p, UmkaStackSlot *r)
{
	umkaGetResult(p, r)->ptrVal = thg;
}

// fn umth_th_getfuncs(): ^struct{}
void
umth_th_getfuncs(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int count = 0;
#define THEXT(ret, name, ...) ++count
#include "thextdata.h"
#undef THEXT

	void **arr = malloc(sizeof(void *) * count);

	int idx = 0;
#define THEXT(ret, name, ...) arr[idx++] = &name
#include "thextdata.h"
#undef THEXT
	umkaGetResult(p, r)->ptrVal = arr;
}

///////////////////////////
// COLOUR

// fn umth_hsv2rgb_uint32(h, s, v, a: th.fu): uint32
static void
umth_hsv2rgb_uint32(UmkaStackSlot *p, UmkaStackSlot *r)
{
	umkaGetResult(p, r)->uintVal =
	    th_color_hsv2rgb(umkaGetParam(p, 0)->real32Val, umkaGetParam(p, 1)->real32Val,
		umkaGetParam(p, 2)->real32Val, umkaGetParam(p, 3)->real32Val);
}

// fn umth_rgb_uint32(r, g, b, a: th.fu): uint32
static void
umth_rgb_uint32(UmkaStackSlot *p, UmkaStackSlot *r)
{
	umkaGetResult(p, r)->uintVal =
	    th_color_rgb(umkaGetParam(p, 0)->real32Val, umkaGetParam(p, 1)->real32Val,
		umkaGetParam(p, 2)->real32Val, umkaGetParam(p, 3)->real32Val);
}

///////////////////////////
// PLACEHOLDERS

extern th_em_placeholder th_em_placeholders[];

// fn umth_placeholder_fetch(id: uint): ^struct{}
static void
umth_placeholder_fetch(UmkaStackSlot *p, UmkaStackSlot *r)
{
	uint32_t id = umkaGetParam(p, 0)->uintVal;

	th_image *img = th_image_alloc();
	th_image_from_data(img, th_em_placeholders[id].data, th_em_placeholders[id].dm);
	umkaGetResult(p, r)->ptrVal = img;
}

///////////////////////////
// FONT

// fn umth_ttf_font_load(out: ^TtfFont, path: str, size: real, filter: uint32): th::ErrCode
static void
umth_ttf_font_load(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_font **ft = umkaGetParam(p, 0)->ptrVal;
	char *path = conv_path(umkaGetParam(p, 1)->ptrVal);
	double size = umkaGetParam(p, 2)->real32Val;
	uint32_t filter = umkaGetParam(p, 3)->uintVal;

	umkaGetResult(p, r)->intVal = th_font_load(ft, path, size, filter);
	free(path);
}

// fn umth_ttf_font_draw(font: TtfFont, s: str, x: real, y: real, color: uint32, scale: real)
static void
umth_ttf_font_draw(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_font *font = umkaGetParam(p, 0)->ptrVal;
	const char *s = umkaGetParam(p, 1)->ptrVal;
	double x = umkaGetParam(p, 2)->real32Val;
	double y = umkaGetParam(p, 3)->real32Val;
	uint32_t color = umkaGetParam(p, 4)->uintVal;
	double scale = umkaGetParam(p, 5)->real32Val;

	th_font_draw(font, s, x, y, color, scale);
}

// fn umth_ttf_font_measure(font: TtfFont, s: str): th::Vf2
static void
umth_ttf_font_measure(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_font *font = umkaGetParam(p, 0)->ptrVal;
	const char *s = umkaGetParam(p, 1)->ptrVal;

	*(th_vf2 *)umkaGetResult(p, r)->ptrVal = th_font_measure(font, s);
}

///////////////////////
// particles

// fn umth_particles_draw(emitter: ^Particles, t: int)
static void
umth_particles_draw(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_particles *emitter = (th_particles *)umkaGetParam(p, 0)->ptrVal;
	int t = umkaGetParam(p, 1)->intVal;

	th_particles_draw(emitter, t);
}

///////////////////////
// tilemaps
// draws a tilemap takes a rectangle as a camera and the tilemap itself

// fn umth_tilemap_draw(t: ^Tilemap)
void
umth_tilemap_draw(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_tmap *t = umkaGetParam(p, 0)->ptrVal;
	th_transform tr = *(th_transform *)umkaGetParam(p, 1);
	th_tmap_draw(t, tr);
}

// checks, if tilemap collides with entity.
// ent - entity to collide with, t - tilemap, x and y - pointers to ints used to return, where the
// collision occured

// fn umth_tilemap_getcoll(tc: ^th::Vf2, ic: ^th::Vf2, t: ^Tilemap, ent: ^Entity): bool
void
umth_tilemap_getcoll(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 *tc = umkaGetParam(p, 0)->ptrVal;
	th_vf2 *ic = umkaGetParam(p, 1)->ptrVal;
	th_tmap *t = umkaGetParam(p, 2)->ptrVal;
	th_ent *ent = umkaGetParam(p, 3)->ptrVal;

	umkaGetResult(p, r)->intVal = th_coll_on_tilemap(ent, t, ic, tc);
}

// fn umth_tilemap_getcoll_line(b: th::Vf2, e: th::Vf2, t: ^Tilemap, ic: ^th::Vf2): bool
void
umth_tilemap_getcoll_line(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 b = *(th_vf2 *)umkaGetParam(p, 0);
	th_vf2 e = *(th_vf2 *)umkaGetParam(p, 1);
	th_tmap *t = umkaGetParam(p, 2)->ptrVal;
	th_vf2 *ic = umkaGetParam(p, 3)->ptrVal;

	umkaGetResult(p, r)->intVal = th_line_to_tilemap(b, e, t, ic);
}

// fn umth_tilemap_autotile(tgt: ^th::uu, w, h: th::uu, src, cfg: ^th::uu, tile: th::uu)
void
umth_tilemap_autotile(UmkaStackSlot *p, UmkaStackSlot *r)
{
	uu *tgt = umkaGetParam(p, 0)->ptrVal;
	uu w = umkaGetParam(p, 1)->uintVal;
	uu h = umkaGetParam(p, 2)->uintVal;
	uu *src = umkaGetParam(p, 3)->ptrVal;
	uu *cfg = umkaGetParam(p, 4)->ptrVal;
	uu tile = umkaGetParam(p, 5)->uintVal;

	th_tmap_autotile(tgt, src, w, h, cfg, tile);
}

///////////////////////
// images
// loads an image at respath + path

// fn umth_image_load(ret: ^Image, path: str): th::ErrCode
void
umth_image_load(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image **img = umkaGetParam(p, 0)->ptrVal;
	char *path = conv_path(umkaGetParam(p, 1)->ptrVal);

	umkaGetResult(p, r)->intVal = th_load_image(img, path);
	free(path);
}

// flips image

// fn umth_image_flipv(i: Image, f: bool)
void
umth_image_flipv(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;

	img->flipv = umkaGetParam(p, 1)->intVal;
}

// flips image

// fn umth_image_fliph(i: Image, f: bool)
void
umth_image_fliph(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;

	img->fliph = umkaGetParam(p, 1)->intVal;
}

// fn umth_image_get_dims(i: Image): th::Vf2
void
umth_image_get_dims(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;

	if (!img)
		return;

	*(th_vf2 *)umkaGetResult(p, r)->ptrVal = img->dm;
}

// fn umth_image_crop(img: Image, tl, br: th::Vf2)
void
umth_image_crop(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;
	th_vf2 tl = *(th_vf2 *)umkaGetParam(p, 1);
	th_vf2 br = *(th_vf2 *)umkaGetParam(p, 2);

	th_image_crop(img, tl, br);
}

// fn umth_image_crop_quad(img: Image, q: th::Quad)
void
umth_image_crop_quad(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;
	img->crop = *(th_quad *)umkaGetParam(p, 1);
}

// fn umth_image_get_crop_quad(img: Image): th::Quad
void
umth_image_get_crop_quad(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;

	*(th_quad *)umkaGetResult(p, r)->ptrVal = img->crop;
}

// returns a pointer to an image from data

// fn umth_image_from_data(ret: ^Image, data: ^uint32, dm: th::Vf2): th::ErrCode
void
umth_image_from_data(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image **ret = umkaGetParam(p, 0)->ptrVal;
	uint32_t *data = umkaGetParam(p, 1)->ptrVal;
	th_vf2 dm = *(th_vf2 *)umkaGetParam(p, 2);

	th_image *img = th_image_alloc();
	if (!img)
		return;

	umkaGetResult(p, r)->intVal = th_image_from_data(img, data, dm);
	*ret = img;
}

// fn umth_image_copy(ret: ^Image, data: Image): th::ErrCode
void
umth_image_copy(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image **ret = umkaGetParam(p, 0)->ptrVal;
	th_image *img1 = umkaGetParam(p, 1)->ptrVal;
	if (!img1)
		return;

	th_image *img2 = th_image_alloc();
	if (!img2)
		return;

	uint32_t *data = th_image_get_data(img1);

	umkaGetResult(p, r)->intVal = th_image_from_data(img2, data, img1->dm);
	img2->flipv = img1->flipv;
	img2->fliph = img1->fliph;
	img2->crop = img1->crop;

	*ret = img2;

	free(data);
}

// fn umth_image_set_filter(data: Image, filter: int): th::ErrCode
void
umth_image_set_filter(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;
	int filter = umkaGetParam(p, 1)->intVal;

	umkaGetResult(p, r)->intVal = th_image_set_filter(img, filter);
}

// fn umth_image_update_data(img: Image, data: ^uint32, dm: th::Vf2): th::ErrCode
void
umth_image_update_data(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;
	uint32_t *data = umkaGetParam(p, 1)->ptrVal;
	th_vf2 dm = *(th_vf2 *)umkaGetParam(p, 2);

	umkaGetResult(p, r)->intVal = th_image_update_data(img, data, dm);
}

// fn umth_image_get_data(img: Image, data: ^uint32)
void
umth_image_get_data(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;
	uint32_t *o = umkaGetParam(p, 1)->ptrVal;

	uint32_t *data = th_image_get_data(img);
	memcpy(o, data, img->dm.w * img->dm.h * sizeof(uint32_t));

	free(data);
}

// fn umth_image_render_target_begin(rt: RenderTarget): th::ErrCode
void
umth_image_render_target_begin(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_render_target *t = umkaGetParam(p, 0)->ptrVal;
	if (!t)
		return;

	umkaGetResult(p, r)->intVal = th_image_set_as_render_target(t);
}

// fn umth_image_render_target_end(rt: RenderTarget, wp: th::Vf2): th::ErrCode
void
umth_image_render_target_end(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_render_target *t = umkaGetParam(p, 0)->ptrVal;
	th_vf2 wp = *(th_vf2 *)umkaGetParam(p, 1);

	umkaGetResult(p, r)->intVal = th_image_remove_render_target(t, wp);
}

// fn umth_image_draw(img: Image, t: th::Transform, color: uint32)
void
umth_image_draw(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;

	th_transform t = *(th_transform *)umkaGetParam(p, 1);
	uint32_t color = umkaGetParam(p, 2)->uintVal;

	th_image_render_transformed(img, t, color);
}

// fn umth_image_blit(img: Image, src, dest: rect::Rect, color: uint32, rot: th::fu, origin:
// th::Vf2)
void
umth_image_blit(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;

	th_rect src = *(th_rect *)umkaGetParam(p, 1);
	th_rect dest = *(th_rect *)umkaGetParam(p, 2);
	uint32_t color = umkaGetParam(p, 3)->uintVal;
	float rot = umkaGetParam(p, 4)->real32Val;
	th_vf2 origin = *(th_vf2 *)umkaGetParam(p, 5);

	th_image_blit(img, src, dest, color, rot, origin);
}

// fn umth_image_draw_on_quad(img: Image, color: uint32, q: th::Quad)
void
umth_image_draw_on_quad(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;

	uint32_t filter = umkaGetParam(p, 1)->uintVal;
	th_quad q = *(th_quad *)umkaGetParam(p, 2);

	th_image_draw_quad(img, q, filter);
}

// fn umth_image_draw_nine_patch(img: Image, outer, inner, dest: rect::Rect,
//                               color: uint32, scale: real)
void
umth_image_draw_nine_patch(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = umkaGetParam(p, 0)->ptrVal;
	if (!img)
		return;

	th_rect outer = *(th_rect *)umkaGetParam(p, 1);
	th_rect inner = *(th_rect *)umkaGetParam(p, 2);
	th_rect dest = *(th_rect *)umkaGetParam(p, 3);

	uint32_t tint = umkaGetParam(p, 4)->uintVal;
	fu scale = umkaGetParam(p, 5)->realVal;

	if (dest.w < 0) {
		dest.x += dest.w;
		dest.w *= -1;
	}

	if (dest.h < 0) {
		dest.y += dest.h;
		dest.h *= -1;
	}

	th_vf2 stepSrc[3] = {{{inner.x, inner.y}}, {{inner.w, inner.h}},
	    {{outer.w - (inner.x + inner.w), outer.h - (inner.y + inner.h)}}};
	th_vf2 stepDst[3] = {{{stepSrc[0].x * scale, stepSrc[0].y * scale}},
	    {{dest.w - stepSrc[0].x * scale - stepSrc[2].x * scale,
		dest.h - stepSrc[0].y * scale - stepSrc[2].y * scale}},
	    {{stepSrc[2].x * scale, stepSrc[2].y * scale}}};

	th_vf2 src = {{outer.x, outer.y}};
	th_vf2 dst = {{dest.x, dest.y}};

	th_vf2 imgDims = img->dm;

	// failsafe
	if (imgDims.x == 0 || imgDims.y == 0) {
		return;
	}

	float mx = 1;
	float my = 1;

	if (img->flipv) {
		mx = -1;
		src.x += outer.w - stepSrc[2].x;
		float temp = stepDst[0].x;
		stepDst[0].x = stepDst[2].x;
		stepDst[2].x = temp;
	}

	if (img->fliph) {
		my = -1;
		src.y += outer.h - stepSrc[2].y;
		float temp = stepDst[0].y;
		stepDst[0].y = stepDst[2].y;
		stepDst[2].y = temp;
	}

	float startY = src.y;

	for (int x = 0; x < 3; x++) {
		float ssX = stepSrc[x].x;
		float sdX = stepDst[x].x;
		for (int y = 0; y < 3; y++) {
			float ssY = stepSrc[y].y;
			float sdY = stepDst[y].y;

			th_rect srcr = {src.x, src.y, ssX, ssY};
			th_rect dstr = {dst.x, dst.y, sdX, sdY};

			th_image_blit(img, srcr, dstr, tint, 0, (th_vf2){0});

			src.y += ssY * my;
			dst.y += sdY;
		}
		src.x += ssX * mx;
		dst.x += sdX;
		src.y = startY;
		dst.y = dest.y;
	}
}

// fn umth_image_create_render_target(ret: ^RenderTarget, width: int, height: int, filter: int):
// th::ErrCode
void
umth_image_create_render_target(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_render_target **result = umkaGetParam(p, 0)->ptrVal;
	int width = umkaGetParam(p, 1)->intVal;
	int height = umkaGetParam(p, 2)->intVal;
	int filter = umkaGetParam(p, 3)->intVal;

	umkaGetResult(p, r)->intVal = th_image_create_render_target(result, width, height, filter);
}

// fn umth_image_render_target_to_image(rt: RenderTarget): Image
void
umth_image_render_target_to_image(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_render_target *rt = umkaGetParam(p, 0)->ptrVal;

	*(th_image **)umkaGetResult(p, r)->ptrVal = rt->image;
}

///////////////////////
// input

// fn umth_input_get_mouse(): th::Vf2
void
umth_input_get_mouse(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 out = {.x = (thg->mouse.x - thg->offset.x) / thg->scaling,
	    .y = (thg->mouse.y - thg->offset.y) / thg->scaling};

	*(th_vf2 *)umkaGetResult(p, r)->ptrVal = out;
}

// fn umth_input_is_pressed(key: Key): bool
void
umth_input_is_pressed(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int keycode = umkaGetParam(p, 0)->intVal;

	umkaGetResult(p, r)->intVal = !!thg->pressed[keycode];
}

// fn umth_input_is_just_pressed(key: Key): bool
void
umth_input_is_just_pressed(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int keycode = umkaGetParam(p, 0)->intVal;

	umkaGetResult(p, r)->intVal = !!thg->just_pressed[keycode];
}

// fn umth_input_is_pressed_repeat(key: Key): bool
void
umth_input_is_pressed_repeat(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int keycode = umkaGetParam(p, 0)->intVal;

	umkaGetResult(p, r)->intVal = !!thg->press_repeat[keycode];
}

// fn umth_input_is_just_released(key: Key): bool
void
umth_input_is_just_released(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int keycode = umkaGetParam(p, 0)->intVal;

	umkaGetResult(p, r)->intVal = !!thg->just_released[keycode];
}

// fn umth_input_clear*(code: Key)
void
umth_input_clear(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int keycode = umkaGetParam(p, 0)->intVal;

	thg->just_pressed[keycode] = 0;
	thg->just_released[keycode] = 0;
	thg->pressed[keycode] = 0;
	thg->press_repeat[keycode] = 0;
}

// fn umth_input_get_str(): str
void
umth_input_get_str(UmkaStackSlot *p, UmkaStackSlot *r)
{
	thg->input_string[thg->input_string_len] = 0;
	umkaGetResult(p, r)->ptrVal = umkaMakeStr(thg->umka, thg->input_string);
}

// fn umth_input_get_mouse_delta(): th::Vf2
void
umth_input_get_mouse_delta(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 out = {
	    .x = thg->mouse_delta.x / thg->scaling, .y = thg->mouse_delta.y / thg->scaling};

	*(th_vf2 *)umkaGetResult(p, r)->ptrVal = out;
}

// fn umth_input_get_mouse_scroll(): th::Vf2
void
umth_input_get_mouse_scroll(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 out = {.x = thg->mouse_wheel.x, .y = thg->mouse_wheel.y};

	*(th_vf2 *)umkaGetResult(p, r)->ptrVal = out;
}

// fn umth_input_gamepad_get_gamepads*(): [4]int
void
umth_input_gamepad_get_gamepads(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int64_t states[4] = {
	    thg->gamepad[0].connected ? 0 : -1,
	    thg->gamepad[1].connected ? 1 : -1,
	    thg->gamepad[2].connected ? 2 : -1,
	    thg->gamepad[3].connected ? 3 : -1,
	};

	memcpy(umkaGetResult(p, r)->ptrVal, states, sizeof(states));
}

// fn umth_input_gamepad_get_gamepad*(): int
void
umth_input_gamepad_get_gamepad(UmkaStackSlot *p, UmkaStackSlot *r)
{
	for (int i = 0; i < 4; i++) {
		if (thg->gamepad[i].connected) {
			umkaGetResult(p, r)->intVal = i;
			return;
		}
	}

	umkaGetResult(p, r)->intVal = -1;
}

// fn umth_input_gamepad_is_pressed(gamepad: int, button: GamepadButton): bool
void
umth_input_gamepad_is_pressed(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int gamepad = umkaGetParam(p, 0)->intVal;
	int button = umkaGetParam(p, 1)->intVal;

	if (gamepad < 0 || gamepad >= 4) {
		umkaGetResult(p, r)->intVal = 0;
		return;
	}

	umkaGetResult(p, r)->intVal = thg->gamepad[gamepad].buttons[button].pressed;
}

// fn umth_input_gamepad_is_just_pressed(gamepad: int, button: GamepadButton): bool
void
umth_input_gamepad_is_just_pressed(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int gamepad = umkaGetParam(p, 0)->intVal;
	int button = umkaGetParam(p, 1)->intVal;

	if (gamepad < 0 || gamepad >= 4) {
		umkaGetResult(p, r)->intVal = 0;
		return;
	}

	umkaGetResult(p, r)->intVal = thg->gamepad[gamepad].buttons[button].just_pressed;
}

// fn umth_input_gamepad_is_just_released(gamepad: int, button: GamepadButton): bool
void
umth_input_gamepad_is_just_released(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int gamepad = umkaGetParam(p, 0)->intVal;
	int button = umkaGetParam(p, 1)->intVal;

	if (gamepad < 0 || gamepad >= 4) {
		umkaGetResult(p, r)->intVal = 0;
		return;
	}

	umkaGetResult(p, r)->intVal = thg->gamepad[gamepad].buttons[button].just_released;
}

// fn umth_input_gamepad_pressure(gamepad: int, button: GamepadButton): th::fu
void
umth_input_gamepad_pressure(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int gamepad = umkaGetParam(p, 0)->intVal;
	int button = umkaGetParam(p, 1)->intVal;

	if (gamepad < 0 || gamepad >= 4) {
		umkaGetResult(p, r)->realVal = 0;
		return;
	}

	umkaGetResult(p, r)->realVal = thg->gamepad[gamepad].buttons[button].pressure;
}

// fn umth_input_gamepad_stick(gamepad: int, stick: GamepadStick): th::Vf2
void
umth_input_gamepad_stick(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int gamepad = umkaGetParam(p, 0)->intVal;
	int stick = umkaGetParam(p, 1)->intVal;

	if (gamepad < 0 || gamepad >= 4) {
		*(th_vf2 *)umkaGetResult(p, r)->ptrVal = (th_vf2){0};
		return;
	}

	switch (stick) {
	case 0: *(th_vf2 *)umkaGetResult(p, r)->ptrVal = thg->gamepad[gamepad].left_stick; break;
	case 1: *(th_vf2 *)umkaGetResult(p, r)->ptrVal = thg->gamepad[gamepad].right_stick; break;
	}
}

// fn umth_input_gamepad_rumble(gamepad: int, left, right: th::fu)
void
umth_input_gamepad_rumble(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int gamepad = umkaGetParam(p, 0)->intVal;
	float left = umkaGetParam(p, 1)->realVal;
	float right = umkaGetParam(p, 2)->realVal;

	if (gamepad < 0 || gamepad >= 4) {
		return;
	}

	if (left < 0)
		left = 0;
	if (right < 0)
		right = 0;
	if (left > 0.99)
		left = 0.99;
	if (right > 0.99)
		right = 0.99;

	thg->gamepad[gamepad].rumble_left = left;
	thg->gamepad[gamepad].rumble_right = right;
}

///////////////////////
// entities
// draws an entity

// fn umth_ent_draw(e: ^Ent)
void
umth_ent_draw(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_ent *e = umkaGetParam(p, 0)->ptrVal;
	th_ent_draw(e);
}

// fn umth_ent_getcoll(maxcolls: uint, e: ^Ent, s: ^[]^Ent, t: ^void): []Coll
void
umth_ent_getcoll(UmkaStackSlot *p, UmkaStackSlot *r)
{
	size_t maxcolls = umkaGetParam(p, 0)->uintVal;
	th_ent *e = umkaGetParam(p, 1)->ptrVal;
	UmkaDynArray(th_ent *) *s = umkaGetParam(p, 2)->ptrVal;
	void *t = umkaGetParam(p, 3)->ptrVal;
	size_t count = umkaGetDynArrayLen(s);
	if (maxcolls == 0) {
		maxcolls = 1;
	}

	if (maxcolls > count) {
		maxcolls = count;
	}

	th_coll *colls = malloc(sizeof(th_coll) * maxcolls);

	uu collC = 0;
	th_ent_getcoll(e, s->data, count, &collC, maxcolls, colls);

	UmkaDynArray(th_coll) *result = umkaGetResult(p, r)->ptrVal;
	umkaMakeDynArray(thg->umka, result, t, collC);
	memcpy(result->data, colls, collC * sizeof(th_coll));

	free(colls);
}

static int
_th_ysort_test(const void *a, const void *b)
{
	return ((th_ent *)a)->t.pos.y - ((th_ent *)b)->t.pos.y;
}

// fn umth_ent_ysort(ents: ^Ent, count: int)
void
umth_ent_ysort(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_ent *ents = (th_ent *)umkaGetParam(p, 0)->ptrVal;
	int count = umkaGetParam(p, 1)->intVal;

	qsort(ents, count, sizeof(th_ent), _th_ysort_test);
}

///////////////////////
// audio

// fn umth_sound_load(Sound: ^Sound, path: str, flags: LoadFlag): th::ErrCode
void
umth_sound_load(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound **out = umkaGetParam(p, 0)->ptrVal;
	char *path = conv_path(umkaGetParam(p, 1)->ptrVal);
	uint32_t flags = umkaGetParam(p, 2)->uintVal;

	umkaGetResult(p, r)->intVal = th_audio_load(out, path, flags);
	free(path);
}

// fn umth_sound_copy(out: ^Sound, s: Sound): th::ErrCode
void
umth_sound_copy(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound **out = umkaGetParam(p, 0)->ptrVal;
	th_sound *s = umkaGetParam(p, 1)->ptrVal;

	umkaGetResult(p, r)->intVal = th_sound_copy(out, s);
}

// fn umth_sound_is_playing(s: Sound): bool
void
umth_sound_is_playing(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;

	umkaGetResult(p, r)->intVal = ma_sound_is_playing(&s->inst);
}

// fn umth_sound_play(s: Sound)
void
umth_sound_play(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;

	ma_sound_start(&s->inst);
}

// fn umth_sound_stop(s: Sound)
void
umth_sound_stop(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;

	ma_sound_stop(&s->inst);
}

// fn umth_sound_set_volume(s: Sound, vol: real32)
void
umth_sound_set_volume(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;
	float vol = umkaGetParam(p, 1)->real32Val;

	ma_sound_set_volume(&s->inst, vol);
}

// fn umth_sound_set_pan(s: Sound, pan: real32)
void
umth_sound_set_pan(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;
	float pan = umkaGetParam(p, 1)->real32Val;

	ma_sound_set_pan(&s->inst, pan);
}

// fn umth_sound_set_pitch(s: Sound, pitch: real32)
void
umth_sound_set_pitch(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;
	float pitch = umkaGetParam(p, 1)->real32Val;

	ma_sound_set_pitch(&s->inst, pitch);
}

// fn umth_sound_set_looping(s: Sound, looping: bool)
void
umth_sound_set_looping(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;
	bool looping = umkaGetParam(p, 1)->intVal;

	ma_sound_set_looping(&s->inst, looping);
}

// fn umth_sound_seek_to_frame(s: Sound, frame: uint)
void
umth_sound_seek_to_frame(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;
	uu frame = umkaGetParam(p, 1)->uintVal;

	ma_sound_seek_to_pcm_frame(&s->inst, frame);
}

// fn umth_sound_frame_count(s: Sound): uint
void
umth_sound_frame_count(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;
	ma_uint64 frame = 0;

	ma_sound_get_length_in_pcm_frames(&s->inst, &frame);
	umkaGetResult(p, r)->uintVal = frame;
}

// fn umth_sound_length_ms(s: Sound): uint
void
umth_sound_length_ms(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;

	float len = 0;
	ma_sound_get_length_in_seconds(&s->inst, &len);

	umkaGetResult(p, r)->intVal = len * 1000;
}

// fn umth_sound_set_start_time_ms(s: Sound, t: uint)
void
umth_sound_set_start_time_ms(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;
	ma_uint64 t = umkaGetParam(p, 1)->uintVal;

	ma_sound_set_start_time_in_milliseconds(&s->inst, t);
}

// fn umth_sound_set_stop_time_ms(s: Sound, t: uint)
void
umth_sound_set_stop_time_ms(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = umkaGetParam(p, 0)->ptrVal;
	ma_uint64 t = umkaGetParam(p, 1)->uintVal;

	ma_sound_set_stop_time_in_milliseconds(&s->inst, t);
}

///////////////////////
// window

// fn umth_window_setup(title: str, w, h: int)
void
umth_window_setup(UmkaStackSlot *p, UmkaStackSlot *r)
{
	char *title = umkaGetParam(p, 0)->ptrVal;
	int w = umkaGetParam(p, 1)->intVal;
	int h = umkaGetParam(p, 2)->intVal;

	th_window_setup(title, w, h);
}

// fn umth_window_get_dimensions(w, h: ^int32)
void
umth_window_get_dimensions(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int32_t *w = (int32_t *)umkaGetParam(p, 0)->ptrVal;
	int32_t *h = (int32_t *)umkaGetParam(p, 1)->ptrVal;

	th_window_get_dimensions(w, h);
}

// fn umth_window_set_viewport(dm: th::Vf2)
void
umth_window_set_viewport(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 dm = *(th_vf2 *)umkaGetParam(p, 0);
	th_calculate_scaling(dm.w, dm.h);
}

// fn umth_window_is_dpi_enabled(): bool
void
umth_window_is_dpi_enabled(UmkaStackSlot *p, UmkaStackSlot *r)
{
	umkaGetResult(p, r)->intVal = thg->dpi_aware;
}

// fn umth_window_get_dpi_scale(): th::fu
void
umth_window_get_dpi_scale(UmkaStackSlot *p, UmkaStackSlot *r)
{
	umkaGetResult(p, r)->realVal = th_window_dpi_scale();
}

// 0 = other/unknown
// 1 = linux
// 2 = windows
// 3 = macos (unsupported currently)
// 4 = emscripten
// fn umth_window_get_platform_id(): th::Platform
void
umth_window_get_platform_id(UmkaStackSlot *p, UmkaStackSlot *r)
{
#ifdef _WIN32
	umkaGetResult(p, r)->intVal = 2;
#elif __linux__
	umkaGetResult(p, r)->intVal = 1;
#elif defined(__EMSCRIPTEN__)
	umkaGetResult(p, r)->intVal = 4;
#else
	umkaGetResult(p, r)->intVal = 0;
#endif
}

// fn umth_window_set_viewport_offset(s: th::Vf2)
void
umth_window_set_viewport_offset(UmkaStackSlot *p, UmkaStackSlot *r)
{
	thg->wp_offset = *(th_vf2 *)umkaGetParam(p, 0);
}

// fn umth_window_get_viewport_offset(): th::Vf2
void
umth_window_get_viewport_offset(UmkaStackSlot *p, UmkaStackSlot *r)
{
	*(th_vf2 *)umkaGetResult(p, r)->ptrVal = thg->wp_offset;
}

// fn umth_window_set_clipboard(s: str)
void
umth_window_set_clipboard(UmkaStackSlot *p, UmkaStackSlot *r)
{
	char *str = umkaGetParam(p, 0)->ptrVal;

	if (str == NULL) {
		return;
	}

	sapp_set_clipboard_string(str);
}

// fn umth_window_get_clipboard(): str
void
umth_window_get_clipboard(UmkaStackSlot *p, UmkaStackSlot *r)
{
	const char *clip = sapp_get_clipboard_string();
	umkaGetResult(p, r)->ptrVal = umkaMakeStr(thg->umka, clip);
}

// fn umth_window_set_fullscreen(fullscreen: bool)
void
umth_window_set_fullscreen(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_window_set_fullscreen(umkaGetParam(p, 0)->uintVal);
}

void
umth_window_get_fullscreen(UmkaStackSlot *p, UmkaStackSlot *r)
{
	umkaGetResult(p, r)->uintVal = th_window_is_fullscreen();
}

// fn umth_window_set_title(title: str)
void
umth_window_set_title(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_window_set_title(umkaGetParam(p, 0)->ptrVal);
}

// fn umth_window_set_dims(dm: th::Vf2)
void
umth_window_set_dims(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 dm = *(th_vf2 *)umkaGetParam(p, 0);
	th_window_set_dims(dm);
}

// fn umth_window_set_icon(img: image::Image)
void
umth_window_set_icon(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_image *img = (th_image *)umkaGetParam(p, 0)->ptrVal;

	th_window_set_icon(img);
}

// fn umth_window_show_cursor(show: bool)
void
umth_window_show_cursor(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_window_show_cursor(umkaGetParam(p, 0)->uintVal);
}

// fn umth_window_freeze_cursor(freeze: bool)
void
umth_window_freeze_cursor(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_window_freeze_cursor(umkaGetParam(p, 0)->uintVal);
}

// fn umth_window_set_cursor(cursor: Cursor)
void
umth_window_set_cursor(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_window_set_cursor(umkaGetParam(p, 0)->uintVal);
}

// fn umth_window_request_exit()
void
umth_window_request_exit(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_window_request_exit();
}

// fn umth_window_set_target_fps(fps: int)
extern int *th_sapp_swap_interval;
void
umth_window_set_target_fps(UmkaStackSlot *p, UmkaStackSlot *r)
{
	int fps = umkaGetParam(p, 0)->intVal;
	if (fps < 0) {
		fps = 0;
	}
	*th_sapp_swap_interval = fps;
}

///////////////////////
// canvas

// fn umth_canvas_draw_text(text: str, pos: th::Vf2, color: uint32, size: th::fu)
void
umth_canvas_draw_text(UmkaStackSlot *p, UmkaStackSlot *r)
{
	char *text = umkaGetParam(p, 0)->ptrVal;
	th_vf2 pos = *(th_vf2 *)umkaGetParam(p, 1);
	uint32_t color = umkaGetParam(p, 2)->uintVal;
	fu size = umkaGetParam(p, 3)->real32Val;

	th_canvas_text(text, color, pos, size);
}

// fn umth_canvas_draw_rect(color: uint32, r: rect::Rect)
void
umth_canvas_draw_rect(UmkaStackSlot *p, UmkaStackSlot *r)
{
	uint32_t color = umkaGetParam(p, 0)->uintVal;
	th_rect re = *(th_rect *)umkaGetParam(p, 1);
	th_canvas_rect(color, re);
}

// fn umth_canvas_draw_line(color: uint32, b, e: th::Vf2, thickness: th::fu)
void
umth_canvas_draw_line(UmkaStackSlot *p, UmkaStackSlot *r)
{
	uint32_t color = umkaGetParam(p, 0)->uintVal;
	th_vf2 b = *(th_vf2 *)umkaGetParam(p, 1);
	th_vf2 e = *(th_vf2 *)umkaGetParam(p, 2);
	float thickness = umkaGetParam(p, 3)->real32Val;

	th_canvas_line(color, b, e, thickness);
}

// fn umth_canvas_draw_rect_lines(color: uint32, r: rect::Rect, thickness: real32)
void
umth_canvas_draw_rect_lines(UmkaStackSlot *p, UmkaStackSlot *r)
{
	uint32_t color = umkaGetParam(p, 0)->uintVal;
	th_rect rect = *(th_rect *)umkaGetParam(p, 1);
	float thickness = umkaGetParam(p, 2)->real32Val;

	th_canvas_draw_rect_lines(color, rect, thickness);
}

// fn umth_canvas_draw_quad(color: uint32, q: th::Quad)
void
umth_canvas_draw_quad(UmkaStackSlot *p, UmkaStackSlot *r)
{
	uint32_t color = umkaGetParam(p, 0)->uintVal;
	th_quad *q = (th_quad *)umkaGetParam(p, 1);

	th_canvas_quad(q, color);
}

// fn umth_canvas_begin_scissor_rect(r: rect::Rect)
void
umth_canvas_begin_scissor_rect(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_canvas_begin_scissor_rect(*(th_rect *)umkaGetParam(p, 0));
}

// fn umth_canvas_end_scissor()
void
umth_canvas_end_scissor(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_canvas_end_scissor();
}

// fn umth_transform_rect(r: Rect, t: th::Transform): th::Quad
void
umth_transform_rect(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_rect rc = *(th_rect *)umkaGetParam(p, 0);
	th_transform t = *(th_transform *)umkaGetParam(p, 1);

	th_transform_rect(umkaGetResult(p, r)->ptrVal, t, rc);
}

// fn umth_transform_quad(q: Quad, t: Transform): Quad
void
umth_transform_quad(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_quad q = *(th_quad *)umkaGetParam(p, 0);
	th_transform t = *(th_transform *)umkaGetParam(p, 1);

	th_transform_quad(&q, t);

	*(th_quad *)umkaGetResult(p, r)->ptrVal = q;
}

// fn umth_transform_vf2(v: Vf2, t: Transform): Vf2
void
umth_transform_vf2(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 v = *(th_vf2 *)umkaGetParam(p, 0);
	th_transform t = *(th_transform *)umkaGetParam(p, 1);

	th_transform_vf2(&v, t);

	*(th_vf2 *)umkaGetResult(p, r)->ptrVal = v;
}

// fn umth_transform_transform(o, t: Transform): Transform
void
umth_transform_transform(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_transform o = *(th_transform *)umkaGetParam(p, 0);
	th_transform t = *(th_transform *)umkaGetParam(p, 1);

	th_transform_transform(&o, t);

	*(th_transform *)umkaGetResult(p, r)->ptrVal = o;
}

// fn umth_quad_max(q: Quad): Vf2
void
umth_quad_max(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_quad q = *(th_quad *)umkaGetParam(p, 0);

	*(th_vf2 *)umkaGetResult(p, r)->ptrVal = th_quad_max(q);
}

// fn umth_quad_min(q: Quad): Vf2
void
umth_quad_min(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_quad q = *(th_quad *)umkaGetParam(p, 0);

	*(th_vf2 *)umkaGetResult(p, r)->ptrVal = th_quad_min(q);
}

// TODO: This isn't in th.um right now, as it would create a cyclic dependency between rect.um and
// th.um
// fn umth_quad_bounding_box(q: Quad): Rect
void
umth_quad_bounding_box(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_quad q = *(th_quad *)umkaGetParam(p, 0);

	*(th_rect *)umkaGetResult(p, r)->ptrVal = th_quad_bounding_box(q);
}

// fn umth_coll_line_to_line(b1, e1, b2, e2: th::Vf2, ic: ^th::Vf2): bool
void
umth_coll_line_to_line(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 b1 = *(th_vf2 *)umkaGetParam(p, 0);
	th_vf2 e1 = *(th_vf2 *)umkaGetParam(p, 1);
	th_vf2 b2 = *(th_vf2 *)umkaGetParam(p, 2);
	th_vf2 e2 = *(th_vf2 *)umkaGetParam(p, 3);
	th_vf2 *ic = umkaGetParam(p, 4)->ptrVal;

	umkaGetResult(p, r)->intVal = th_line_to_line(b1, e1, b2, e2, ic);
}

// fn umth_coll_point_to_quad(v: th::Vf2, q: th::Quad, ic: ^th::Vf2): bool
void
umth_coll_point_to_quad(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 v = *(th_vf2 *)umkaGetParam(p, 0);
	th_quad q = *(th_quad *)umkaGetParam(p, 1);
	th_vf2 *ic = umkaGetParam(p, 2)->ptrVal;

	umkaGetResult(p, r)->intVal = th_point_to_quad(v, &q, ic);
}

// fn umth_coll_line_to_quad(b, e: th::Vf2, q: th::Quad, ic1, ic2: ^th::Vf2): bool
void
umth_coll_line_to_quad(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 b = *(th_vf2 *)umkaGetParam(p, 0);
	th_vf2 e = *(th_vf2 *)umkaGetParam(p, 1);
	th_quad q = *(th_quad *)umkaGetParam(p, 2);
	th_vf2 *ic1 = umkaGetParam(p, 3)->ptrVal;
	th_vf2 *ic2 = umkaGetParam(p, 4)->ptrVal;

	umkaGetResult(p, r)->intVal = th_line_to_quad(b, e, &q, ic1, ic2);
}

// fn umth_coll_quad_to_quad(q1, q2: th::Quad, ic: ^th::Vf2): bool
void
umth_coll_quad_to_quad(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_quad q1 = *(th_quad *)umkaGetParam(p, 0);
	th_quad q2 = *(th_quad *)umkaGetParam(p, 1);
	th_vf2 *ic = umkaGetParam(p, 2)->ptrVal;

	umkaGetResult(p, r)->intVal = th_quad_to_quad(&q1, &q2, ic);
}

// fn umth_coll_point_to_rect(p: th::Vf2, r: rect::Rect): bool
void
umth_coll_point_to_rect(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_vf2 v = *(th_vf2 *)umkaGetParam(p, 0);
	th_rect rc = *(th_rect *)umkaGetParam(p, 1);

	umkaGetResult(p, r)->intVal = th_coll_point_on_rect(v, &rc);
}

// fn umth_coll_rect_to_rect(r1, r2: rect::Rect): bool
void
umth_coll_rect_to_rect(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_rect r1 = *(th_rect *)umkaGetParam(p, 0);
	th_rect r2 = *(th_rect *)umkaGetParam(p, 1);

	umkaGetResult(p, r)->intVal = th_rect_to_rect(&r1, &r2);
}

// fn umth_coll_rect_intersect(r1, r2: rect::Rect): rect::Rect
void
umth_coll_rect_intersect(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_rect r1 = *(th_rect *)umkaGetParam(p, 0);
	th_rect r2 = *(th_rect *)umkaGetParam(p, 1);

	*(th_rect *)umkaGetResult(p, r)->ptrVal = th_rect_intersect(r1, r2);
}

// fn umth_nav_mesh_add_quad(m: ^Mesh, q: th::Quad)
void
umth_nav_mesh_add_quad(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_navmesh *m = umkaGetParam(p, 0)->ptrVal;
	th_quad q = *(th_quad *)umkaGetParam(p, 1);

	th_navmesh_add_quad(m, &q);
}

// fn umth_nav_mesh_nav(t: ^void, m: ^Mesh, p1, p2: th::Vf2): []th::Vf2
void
umth_nav_mesh_nav(UmkaStackSlot *p, UmkaStackSlot *r)
{
	void *cameFromType = umkaGetParam(p, 0)->ptrVal;
	th_navmesh *m = umkaGetParam(p, 1)->ptrVal;
	th_vf2 p1 = *(th_vf2 *)umkaGetParam(p, 2);
	th_vf2 p2 = *(th_vf2 *)umkaGetParam(p, 3);

	th_vf2s *cameFrom = umkaGetResult(p, r)->ptrVal;

	th_navmesh_nav(cameFrom, cameFromType, m, p1, p2);
}

// fn umth_atlas_pack(a: ^Atlas, images: ^[]image::Image, strategy: int): th::ErrCode
void
umth_atlas_pack(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_atlas *a = umkaGetParam(p, 0)->ptrVal;
	UmkaDynArray(th_image *) *images = umkaGetParam(p, 1)->ptrVal;
	th_atlas_pack_strategy strategy = umkaGetParam(p, 2)->intVal;

	umkaGetResult(p, r)->intVal = th_atlas_pack(a, images, strategy);
}

void
_th_umka_bind(void *umka)
{
	// etc
	umkaAddFunc(umka, "umth_fopen", &umth_fopen);
	umkaAddFunc(umka, "rtlfopen", &umth_fopen);
	umkaAddFunc(umka, "umth_th_getglobal", &umth_th_getglobal);
	umkaAddFunc(umka, "umth_th_getfuncs", &umth_th_getfuncs);

	// color
	umkaAddFunc(umka, "umth_hsv2rgb_uint32", &umth_hsv2rgb_uint32);
	umkaAddFunc(umka, "umth_rgb_uint32", &umth_rgb_uint32);

	// font
	umkaAddFunc(umka, "umth_ttf_font_load", &umth_ttf_font_load);
	umkaAddFunc(umka, "umth_ttf_font_draw", &umth_ttf_font_draw);
	umkaAddFunc(umka, "umth_ttf_font_measure", &umth_ttf_font_measure);

	// particles
	umkaAddFunc(umka, "umth_particles_draw", &umth_particles_draw);

	// placeholders
	umkaAddFunc(umka, "umth_placeholder_fetch", &umth_placeholder_fetch);

	// tilemaps
	umkaAddFunc(umka, "umth_tilemap_draw", &umth_tilemap_draw);
	umkaAddFunc(umka, "umth_tilemap_getcoll", &umth_tilemap_getcoll);
	umkaAddFunc(umka, "umth_tilemap_getcoll_line", &umth_tilemap_getcoll_line);
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
	umkaAddFunc(umka, "umth_image_render_target_begin", &umth_image_render_target_begin);
	umkaAddFunc(umka, "umth_image_render_target_end", &umth_image_render_target_end);
	umkaAddFunc(umka, "umth_image_draw", &umth_image_draw);
	umkaAddFunc(umka, "umth_image_blit", &umth_image_blit);
	umkaAddFunc(umka, "umth_image_draw_on_quad", &umth_image_draw_on_quad);
	umkaAddFunc(umka, "umth_image_draw_nine_patch", &umth_image_draw_nine_patch);
	umkaAddFunc(umka, "umth_image_create_render_target", &umth_image_create_render_target);
	umkaAddFunc(umka, "umth_image_render_target_to_image", &umth_image_render_target_to_image);

	// input
	umkaAddFunc(umka, "umth_input_get_mouse", &umth_input_get_mouse);
	umkaAddFunc(umka, "umth_input_is_pressed", &umth_input_is_pressed);
	umkaAddFunc(umka, "umth_input_is_just_pressed", &umth_input_is_just_pressed);
	umkaAddFunc(umka, "umth_input_is_pressed_repeat", &umth_input_is_pressed_repeat);
	umkaAddFunc(umka, "umth_input_is_just_released", &umth_input_is_just_released);
	umkaAddFunc(umka, "umth_input_clear", &umth_input_clear);
	umkaAddFunc(umka, "umth_input_get_str", &umth_input_get_str);
	umkaAddFunc(umka, "umth_input_get_mouse_delta", &umth_input_get_mouse_delta);
	umkaAddFunc(umka, "umth_input_get_mouse_scroll", &umth_input_get_mouse_scroll);
	umkaAddFunc(umka, "umth_input_gamepad_get_gamepads", &umth_input_gamepad_get_gamepads);
	umkaAddFunc(umka, "umth_input_gamepad_get_gamepad", &umth_input_gamepad_get_gamepad);
	umkaAddFunc(umka, "umth_input_gamepad_is_pressed", &umth_input_gamepad_is_pressed);
	umkaAddFunc(
	    umka, "umth_input_gamepad_is_just_pressed", &umth_input_gamepad_is_just_pressed);
	umkaAddFunc(
	    umka, "umth_input_gamepad_is_just_released", &umth_input_gamepad_is_just_released);
	umkaAddFunc(umka, "umth_input_gamepad_pressure", &umth_input_gamepad_pressure);
	umkaAddFunc(umka, "umth_input_gamepad_stick", &umth_input_gamepad_stick);
	umkaAddFunc(umka, "umth_input_gamepad_rumble", &umth_input_gamepad_rumble);

	// entities
	umkaAddFunc(umka, "umth_ent_draw", &umth_ent_draw);
	umkaAddFunc(umka, "umth_ent_getcoll", &umth_ent_getcoll);
	umkaAddFunc(umka, "umth_ent_ysort", &umth_ent_ysort);

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
	umkaAddFunc(umka, "umth_sound_length_ms", umth_sound_length_ms);
	umkaAddFunc(umka, "umth_sound_set_start_time_ms", &umth_sound_set_start_time_ms);
	umkaAddFunc(umka, "umth_sound_set_stop_time_ms", &umth_sound_set_stop_time_ms);

	// window
	umkaAddFunc(umka, "umth_window_set_viewport_offset", &umth_window_set_viewport_offset);
	umkaAddFunc(umka, "umth_window_get_viewport_offset", &umth_window_get_viewport_offset);
	umkaAddFunc(umka, "umth_window_set_clipboard", &umth_window_set_clipboard);
	umkaAddFunc(umka, "umth_window_get_clipboard", &umth_window_get_clipboard);
	umkaAddFunc(umka, "umth_window_get_fullscreen", &umth_window_get_fullscreen);
	umkaAddFunc(umka, "umth_window_set_fullscreen", &umth_window_set_fullscreen);
	umkaAddFunc(umka, "umth_window_get_dpi_scale", &umth_window_get_dpi_scale);
	umkaAddFunc(umka, "umth_window_is_dpi_enabled", &umth_window_is_dpi_enabled);
	umkaAddFunc(umka, "umth_window_setup", &umth_window_setup);
	umkaAddFunc(umka, "umth_window_get_dimensions", &umth_window_get_dimensions);
	umkaAddFunc(umka, "umth_window_set_viewport", &umth_window_set_viewport);
	umkaAddFunc(umka, "umth_window_set_title", &umth_window_set_title);
	umkaAddFunc(umka, "umth_window_set_dims", &umth_window_set_dims);
	umkaAddFunc(umka, "umth_window_set_icon", &umth_window_set_icon);
	umkaAddFunc(umka, "umth_window_show_cursor", &umth_window_show_cursor);
	umkaAddFunc(umka, "umth_window_freeze_cursor", &umth_window_freeze_cursor);
	umkaAddFunc(umka, "umth_window_set_cursor", &umth_window_set_cursor);
	umkaAddFunc(umka, "umth_window_request_exit", &umth_window_request_exit);
	umkaAddFunc(umka, "umth_window_set_target_fps", &umth_window_set_target_fps);
	umkaAddFunc(umka, "umth_window_get_platform_id", &umth_window_get_platform_id);

	// canvas
	umkaAddFunc(umka, "umth_canvas_draw_text", &umth_canvas_draw_text);
	umkaAddFunc(umka, "umth_canvas_draw_rect", &umth_canvas_draw_rect);
	umkaAddFunc(umka, "umth_canvas_draw_line", &umth_canvas_draw_line);
	umkaAddFunc(umka, "umth_canvas_draw_rect_lines", &umth_canvas_draw_rect_lines);
	umkaAddFunc(umka, "umth_canvas_draw_quad", &umth_canvas_draw_quad);
	umkaAddFunc(umka, "umth_canvas_begin_scissor_rect", &umth_canvas_begin_scissor_rect);
	umkaAddFunc(umka, "umth_canvas_end_scissor", &umth_canvas_end_scissor);

	// transform
	umkaAddFunc(umka, "umth_transform_rect", &umth_transform_rect);
	umkaAddFunc(umka, "umth_transform_quad", &umth_transform_quad);
	umkaAddFunc(umka, "umth_transform_vf2", &umth_transform_vf2);
	umkaAddFunc(umka, "umth_transform_transform", &umth_transform_transform);

	// collisions
	umkaAddFunc(umka, "umth_coll_line_to_line", &umth_coll_line_to_line);
	umkaAddFunc(umka, "umth_coll_point_to_quad", &umth_coll_point_to_quad);
	umkaAddFunc(umka, "umth_coll_line_to_quad", &umth_coll_line_to_quad);
	umkaAddFunc(umka, "umth_coll_quad_to_quad", &umth_coll_quad_to_quad);
	umkaAddFunc(umka, "umth_coll_point_to_rect", &umth_coll_point_to_rect);
	umkaAddFunc(umka, "umth_coll_rect_to_rect", &umth_coll_rect_to_rect);
	umkaAddFunc(umka, "umth_coll_rect_intersect", &umth_coll_rect_intersect);

	// nav
	umkaAddFunc(umka, "umth_nav_mesh_add_quad", &umth_nav_mesh_add_quad);
	umkaAddFunc(umka, "umth_nav_mesh_nav", &umth_nav_mesh_nav);

	// quad
	umkaAddFunc(umka, "umth_quad_min", &umth_quad_min);
	umkaAddFunc(umka, "umth_quad_max", &umth_quad_max);
	umkaAddFunc(umka, "umth_quad_bounding_box", &umth_quad_bounding_box);

	// atlas
	umkaAddFunc(umka, "umth_atlas_pack", &umth_atlas_pack);

	for (int i = 0; i < th_em_modulenames_count; i++) {
		umkaAddModule(umka, th_em_modulenames[i], th_em_modulesrc[i]);
	}
}
