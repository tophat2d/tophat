#ifndef TOPHAT_H
#define TOPHAT_H

#ifdef _WIN32
#include <windows.h>
#define _CRT_SECURE_NO_WARNINGS
#elif defined(__linux__)
#include <X11/Xlib.h>
#endif

#include <miniaudio.h>
#include <sokol_app.h>
#include <sokol_gfx.h>
#include <stb_truetype.h>
#include <stdbool.h>
#include <stdint.h>
#include <umka_api.h>

#define PI 3.1415926535897932

#define INPUT_STRING_SIZE 256
#define MAX_SCISSORS 1024

// each unit here is a triangle, that's why multiplication by 3
// 2 + 2 + 4 is (position + uv + colour)
#define BATCH_SIZE 4096
#define BATCH_VERTEX (2 + 2 + 4)
#define BATCH_UNIT (3 * BATCH_VERTEX)
#define BATCH_LENGTH (BATCH_SIZE * BATCH_UNIT)
#define RENDER_TARGET_MAX_SIZE 1024

typedef float fu;
typedef uint32_t uu;
typedef int32_t iu;

#define LEN(a) (sizeof(a) / sizeof((a)[0]))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define TH_ASSERT(err, e) \
	do { \
		if (!(e)) \
			return err; \
	} while (0)

typedef enum
{
	th_err_ok = 0,
	th_err_failure = 1,
	th_err_io,
	th_err_bad_enum,
	th_err_bad_action,
	th_err_inval,
	th_err_alloc,
	th_err_already,
	th_err_out_of_bounds,
	th_err__pad = 2147483647
} th_err;

typedef union
{
	struct
	{
		fu w, h;
	};
	struct
	{
		fu x, y;
	};
} th_vf2;

typedef UmkaDynArray(th_vf2) th_vf2s;

typedef struct
{
	fu x, y, w, h;
} th_rect;

typedef union
{
	struct
	{
		th_vf2 tl, tr, br, bl;
	};
	th_vf2 v[4];
} th_quad;

typedef struct
{
	th_vf2 pos;
	th_vf2 scale;
	th_vf2 origin;
	fu rot;
} th_transform;

typedef struct
{
	th_vf2 dm;
	uu channels;
	sg_image tex;
	sg_sampler smp;
	sg_filter filter;
	th_quad crop;
	char flipv;
	char fliph;
	bool target;
} th_image;

typedef struct
{
	sg_attachments attachments;
	sg_image depth;
	th_image *image;
} th_render_target;

typedef struct
{
	ma_sound inst;
	bool copied;
} th_sound;

typedef struct
{
	th_rect rect;
	th_image *img;
	th_transform t;
	uint32_t color;
} th_ent;

typedef struct
{
	uu index;
	th_vf2 pos;
} th_coll;

typedef struct
{
	uint64_t start_time;
	int seed;
} _th_particle;

typedef struct
{
	th_vf2 pos;
	th_vf2 dm;
	th_vf2 gravity;
	bool repeat;
	bool active;

	th_vf2 angle;

	uu lifetime;
	fu lifetime_randomness;

	fu velocity;
	fu velocity_randomness;

	fu size;
	fu size_randomness;
	fu max_size;

	fu rotation;
	fu max_rotation;
	fu rotation_randomness;

	UmkaDynArray(uint32_t) colors;

	UmkaDynArray(_th_particle) particles;
} th_particles;

typedef struct
{
	th_image *i;
	th_vf2 cs;
	th_vf2 dm;
} th_atlas;

typedef enum
{
	TH_ATLAS_PACK_SQUARE,
	TH_ATLAS_PACK_ROW,
	TH_ATLAS_PACK_COLUMN
} th_atlas_pack_strategy;

typedef struct
{
	th_atlas a;
	th_vf2 pos;
	uu w;
	UmkaDynArray(uu) cells;
	UmkaDynArray(bool) collmask;
	fu scale;
} th_tmap;

typedef struct
{
	uint32_t *data;
	th_vf2 dm;
} th_em_placeholder;

#ifdef _WIN32
typedef void *th_window_handle;
#elif __linux__
#include <X11/Xlib.h>
typedef Window *th_window_handle;
#elif defined(__EMSCRIPTEN__)
typedef int th_window_handle;
#else
#error Unsupported platform
#endif

#define TH_FONTPLATFORM_PAGECOUNT 1024
#define TH_FONTPLATFORM_CHARSPERPAGE 256

// this fits 256 characters into an img.
// with an nxn image. size is power of 2. don't go over 1024x1024 please.
typedef struct
{
	th_image img; // Can i really safely do that
	stbtt_packedchar pc[TH_FONTPLATFORM_CHARSPERPAGE];
} th_font_atlas_page;

typedef struct
{
	uint32_t filter;
	double size, ascent, descent, scale;

	void *data;
	stbtt_fontinfo info;
	// 1024 is a reasonable amount to represent about 262k glyphs (256*1024)
	// page 0 (ascii set), is always allocated at startup for frequencey reasons.
	th_font_atlas_page *pages[TH_FONTPLATFORM_PAGECOUNT];
} th_font;

typedef uint32_t th_shader;

typedef struct
{
	uu pressed;
	uu just_pressed;
	uu just_released;
	fu pressure;
} th_gamepad_button;

typedef struct
{
	bool connected;

	// Modelled after XBox controller
	union
	{
		struct
		{
			th_gamepad_button A, B, X, Y;
			th_gamepad_button LT, RT, LB, RB;

			th_gamepad_button select; // AKA view, back
			th_gamepad_button start;  // AKA menu

			th_gamepad_button dpad_up;
			th_gamepad_button dpad_down;
			th_gamepad_button dpad_left;
			th_gamepad_button dpad_right;

			th_gamepad_button left_stick_press;
			th_gamepad_button right_stick_press;
		};
		th_gamepad_button buttons[16];
	};

	th_vf2 left_stick;  // -1 to 1
	th_vf2 right_stick; // -1 to 1

	float rumble_left;
	float rumble_right;
} th_generic_gamepad;

typedef struct
{
	int addr;
	UmkaStackSlot *p;
	UmkaStackSlot *r;
} th_umka_func;

// struct holding all tophat's global variables.
typedef struct
{
	char respath[4096];
	fu scaling;
	th_vf2 offset;
	th_vf2 viewport;
	th_vf2 wp_offset;
	th_vf2 target_size;
	void *umka;

	uu pressed[512];
	// since `input::clear` clears `pressed`, there will be no way to detect releases.
	uu internal_pressed_state[512];
	uu just_pressed[512];
	uu just_released[512];
	uu press_repeat[512];
	th_vf2 mouse;
	th_vf2 mouse_delta;
	th_vf2 mouse_wheel;

	th_generic_gamepad gamepad[4];

	th_shader *shaders;
	uu shader_count;

	char input_string[INPUT_STRING_SIZE];
	uu input_string_len;

	float canvas_batch[BATCH_LENGTH];
	uu canvas_batch_size;

	sg_shader main_shader;

	sg_pass_action pass_action;
	sg_bindings canvas_bind;
	sg_pipeline canvas_pip;
	th_image *canvas_image;

	sg_pass_action offscreen_pass_action;
	sg_pipeline image_pip;

	bool has_render_target;

	th_rect scissors[MAX_SCISSORS];
	uu scissor;

	bool prof;
	bool profJson;

	int argc;
	char **argv;
	int argOffset;
	bool silent;
	bool check;
	bool print_asm;
	bool dpi_aware;

	float dpi_scale_factor;

	th_umka_func umka_init;
	th_umka_func umka_destroy;
	th_umka_func umka_frame;

	ma_engine audio_engine;
} th_global;

typedef struct
{
	UmkaDynArray(bool) d;
	th_rect r;
	uu w;
	fu s;
} th_navmesh;

#ifdef THEXT
#undef THEXT
#define THEXT(ret, name, ...) static ret (*name)(__VA_ARGS__) = NULL
#include "thextdata.h"
#undef THEXT

static int
th_ext_count()
{
	int count = 0;
#define THEXT(ret, name, ...) ++count
#include "thextdata.h"
#undef THEXT
	return count;
}

static void
th_ext_set(void **arr)
{
	int count = th_ext_count();

	int idx = 0;
#define THEXT(ret, name, ...) name = arr[idx++]
#include "thextdata.h"
#undef THEXT
}

#define THEXT
#endif

#ifndef THEXT

// atlas
th_err
th_atlas_pack(th_atlas *a, void *arr, th_atlas_pack_strategy strategy);
th_vf2
th_atlas_nth_coords(th_atlas *a, uu n);
th_rect
th_atlas_get_cell(th_atlas *a, th_vf2 cell);

// audio
void
th_audio_init();
void
th_audio_deinit();
th_err
th_audio_load(th_sound **out, char *path, uint32_t flags);
th_err
th_sound_copy(th_sound **out, th_sound *s);

// bindings
void
_th_umka_bind(void *umka);

// canvas
void
th_canvas_rect(uint32_t color, th_rect r);
void
th_canvas_init();
void
th_canvas_line(uint32_t color, th_vf2 f, th_vf2 t, fu thickness);
void
th_canvas_draw_rect_lines(uint32_t color, th_rect rect, fu thickness);
void
th_canvas_text(char *text, uint32_t color, th_vf2 p, fu size);
void
th_canvas_triangle(uint32_t color, th_vf2 a, th_vf2 b, th_vf2 c);
void
th_canvas_quad(th_quad *q, uint32_t color);
bool
th_canvas_batch_push(float *array, size_t n);
void
th_canvas_flush();
void
th_canvas_use_image(th_image *img);
void
th_canvas_batch_push_auto_flush(th_image *img, float *array, size_t n);
void
th_canvas_begin_scissor_rect(th_rect rect);
void
th_canvas_end_scissor();
void
th_canvas_end_frame();

// collisions
int
th_line_to_line(th_vf2 b1, th_vf2 e1, th_vf2 b2, th_vf2 e2, th_vf2 *ic);
uu
th_point_to_quad(th_vf2 p, th_quad *q, th_vf2 *ic);
uu
th_quad_to_quad(th_quad *q1, th_quad *q2, th_vf2 *ic);
uu
th_ent_to_ent(th_ent *e1, th_ent *e2, th_vf2 *ic);
uu
th_line_to_quad(th_vf2 b, th_vf2 e, th_quad *q, th_vf2 *ic1, th_vf2 *ic2);
uu
th_coll_on_tilemap(th_ent *e, th_tmap *t, th_vf2 *ic, th_vf2 *tc);
uu
th_coll_point_on_rect(th_vf2 p, th_rect *r);
uu
th_rect_to_rect(th_rect *r1, th_rect *r2);
uu
th_line_to_tilemap(th_vf2 b, th_vf2 e, th_tmap *t, th_vf2 *ic);
th_rect
th_rect_intersect(th_rect a, th_rect b);

// entity
th_quad
th_ent_transform(th_ent *e);
void
th_ent_draw(th_ent *o);
void
th_ent_getcoll(th_ent *e, th_ent **scene, uu count, uu *collC, uu maxColls, th_coll *colls);

// colour
uint32_t
th_color_rgb(float r, float g, float b, float a);
uint32_t
th_color_hsv2rgb(float h, float s, float v, float a);

// font
th_err
th_font_load(th_font **out, char *path, double size, uint32_t filter);
void
th_font_draw(th_font *font, const char *s, double x, double y, uint32_t color, double scale);
th_vf2
th_font_measure(th_font *font, const char *s);
void
th_font_deinit();

// image
th_err
th_load_image(th_image **out, char *path);
void
th_image_free(th_image *img);
th_err
th_image_from_data(th_image *img, uint32_t *data, th_vf2 dm);
uint32_t *
th_image_get_data(th_image *img);
void
th_image_draw_quad(th_image *img, th_quad q, uint32_t color);
void
th_image_blit(th_image *img, th_rect src, th_rect dest, uint32_t color, float rot, th_vf2 origin);
void
th_image_render_transformed(th_image *img, th_transform trans, uint32_t color);
void
th_image_crop(th_image *img, th_vf2 tl, th_vf2 br);

th_err
th_image_set_filter(th_image *img, sg_filter filter);
th_err
th_image_update_data(th_image *img, uint32_t *data, th_vf2 dm);
th_image *
th_image_alloc();
void
th_image_init();
void
th_image_deinit();

th_err
th_image_create_render_target(th_render_target **out, int width, int height, int filter);
th_err
th_image_set_as_render_target(th_render_target *t);
th_err
th_image_remove_render_target(th_render_target *t, th_vf2 wp);

// input
void
th_input_key(int keycode, int bDown);
void
th_input_repeated(int keycode, int bDown);
void
th_input_modifiers(uint32_t modifiers);
void
th_input_cycle();
void
th_input_reset();
void
th_input_update_gamepads();

// misc
void
th_error(char *text, ...);
void
th_info(char *text, ...);
void
th_calculate_scaling(float camw, float camh);
int
th_init(const char *scriptpath, const char *script_path);
void
th_deinit();
void
th_print_umka_error_and_quit(int code);
void
th_regularize_path(const char *path, const char *cur_folder, char *regularized_path, int size);

// navmesh
void
th_navmesh_add_quad(th_navmesh *m, th_quad *q);
th_err
th_navmesh_nav(th_vf2s *cameFrom, void *cameFromType, th_navmesh *m, th_vf2 p1, th_vf2 p2);
void
th_nav_init(void);

// particles
void
th_particles_draw(th_particles *p, int t);

// quad/transform
th_vf2
th_quad_min(th_quad q);
th_vf2
th_quad_max(th_quad q);
th_rect
th_quad_bounding_box(th_quad q);
void
th_transform_rect(th_quad *q, th_transform t, th_rect r);
void
th_transform_quad(th_quad *q, th_transform t);
void
th_transform_vf2(th_vf2 *v, th_transform t);
void
th_transform_transform(th_transform *o, th_transform t);

// vector
void
th_rotate_point(th_vf2 *p, th_vf2 o, fu rot);
void
th_vector_normalize(float *x, float *y);

// sokol
uint32_t
th_sg_get_gl_image(sg_image img);

// tilemap
void
th_tmap_draw(th_tmap *t);
void
th_tmap_autotile(uu *tgt, uu *src, uu w, uu h, uu *tiles, uu limiter);

// tophat
th_shader *
th_get_shader(uu index);
th_shader *
th_get_shader_err(uu index);
th_shader *
th_alloc_shader();

// utf8
size_t
th_utf8_decode(uint32_t *out, const char *s);
size_t
th_utf8_encode(char *out, uint32_t r);

// window
fu
th_window_dpi_scale();
void
th_window_setup(char *name, int w, int h);
void
th_window_get_dimensions(int *w, int *h);
th_window_handle
th_get_window_handle();
void
th_window_set_title(const char *);
void
th_window_set_dims(th_vf2 dm);
void
th_window_set_icon(th_image *img);
void
th_window_show_cursor(bool show);
void
th_window_freeze_cursor(bool freeze);
void
th_window_set_cursor(int cursor);
void
th_window_request_exit();
bool
th_window_is_fullscreen();
void
th_window_set_fullscreen(bool fullscreen);
sapp_desc
th_window_sapp_desc();

#endif

#endif
