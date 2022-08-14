#ifndef TOPHAT_H
#define TOPHAT_H

#include <stdbool.h>
#include <miniaudio.h>
#include <stb_truetype.h>
#include <umka_api.h>
#include <GL/gl.h>

#define MAX_SOUNDS 512
#define MAX_IMAGES 2048
#define MAX_FONTS 128

typedef float fu;
typedef unsigned short uu;
typedef short iu;

#define SWAP_VARS(v1, v2, type) { type tmp = v1; v1 = v2; v2 = tmp; }
#define LEN(a) (sizeof(a) / sizeof(a[0]))

typedef union {
	struct {fu w, h;};
	struct {fu x, y;};
} th_vf2;

typedef struct {
	fu x, y, w, h;
} th_rect;

typedef union {
	struct {th_vf2 tl, tr, br, bl;};
	th_vf2 v[4];
} th_quad;

typedef struct {
	th_vf2 pos;
	th_vf2 scale;
	th_vf2 origin;
	fu rot;
} th_transform;

typedef struct {
	bool looping;
	fu volume;
	char *path;
} th_sound;

typedef struct {
	ma_decoder *decoder;
	bool playing;
	bool paused;
	bool looping;
	fu volume;
	int64_t frame;
} th_playback;

typedef struct th_playback_item {
	th_playback *pk;
	struct th_playback_item *next;
} th_playback_item;

typedef struct {
	th_vf2 dm;
	uu channels;
	uint32_t *data;
	unsigned int gltexture;
	uu filter;
	th_rect crop;
	char flipv;
	char fliph;
} th_image;

typedef struct {
	th_rect rect;
	th_image *img;
	th_transform t;
	uint32_t color;
} th_ent;

typedef struct {
	uu index;
	th_vf2 pos;
} th_coll;

typedef struct {
	uint32_t *dots;
	uu w, h;
	fu rect_size;
	uint32_t color;
} th_lightmask;

typedef struct {
	th_vf2 pos;
	uu radius;
	uint32_t tint;
} th_spotlight;

typedef struct {
	uint64_t start_time;
	int seed;
} _th_particle;

typedef struct {
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

typedef struct {
	th_vf2 pos;
	fu l;
	fu r;
} th_ray;

typedef struct {
	th_image *i;
	th_vf2 cs;
	th_vf2 dm;
} th_atlas;

typedef struct {
	th_atlas a;
	th_vf2 pos;
	uu w;
	UmkaDynArray(uu) cells;
	UmkaDynArray(bool) collmask;
	fu scale;
} th_tmap;


#define TH_FONTPLATFORM_PAGECOUNT 1024
#define TH_FONTPLATFORM_CHARSPERPAGE 256

// this fits 256 characters into an img.
// with an nxn image. size is power of 2. don't go over 1024x1024 please.
typedef struct {
	th_image img; // Can i really safely do that
	stbtt_packedchar pc[TH_FONTPLATFORM_CHARSPERPAGE];
} th_font_atlas_page;

typedef struct {
	uint32_t filter;
	double size, ascent, descent, scale;

	void* data;
	stbtt_fontinfo info;
	// 1024 is a reasonable amount to represent about 262k glyphs (256*1024)
	// page 0 (ascii set), is always allocated at startup for frequencey reasons.
	th_font_atlas_page *pages[TH_FONTPLATFORM_PAGECOUNT];
} th_font;

typedef GLuint th_shader;

// struct holding all tophat's global variables.
typedef struct {
	char respath[4096];
	fu scaling;
	th_vf2 offset;
	void *umka;

	uu pressed[255];
	uu just_pressed[255];
	th_vf2 mouse;

	th_playback_item *playbacks;

	th_font *fonts;
	uu font_count;

	th_shader *shaders;
	uu shader_count;
} th_global;

// atlas
th_vf2 th_atlas_nth_coords(th_atlas *a, uu n);
th_rect th_atlas_get_cell(th_atlas *a, th_vf2 cell);

// audio
void th_audio_init();
void th_audio_deinit();
void _th_audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

// bindings
void _th_umka_bind(void *umka);

// canvas
int th_canvas_compile_shader(char *frag, char *vert);
void th_canvas_rect(uint32_t color, th_rect r);
void th_canvas_init();
void th_canvas_line(uint32_t color, th_vf2 f, th_vf2 t, fu thickness);
void th_canvas_text(char *text, uint32_t color, th_vf2 p, fu size);
void th_canvas_triangle(uint32_t color, th_vf2 a, th_vf2 b, th_vf2 c);
void th_canvas_flush();

// collisions
int th_line_to_line(th_vf2 b1, th_vf2 e1, th_vf2 b2, th_vf2 e2, th_vf2 *ic);
uu th_point_to_quad(th_vf2 p, th_quad *q, th_vf2 *ic);
uu th_ent_to_ent(th_ent *e1, th_ent *e2, th_vf2 *ic);
uu th_line_to_quad(th_vf2 b, th_vf2 e, th_quad *q, th_vf2 *ic);
uu _th_coll_on_tilemap(th_ent *e, th_tmap *t, uu *vert, th_vf2 *tc);
bool th_ray_to_tilemap(th_ray *ra, th_tmap *t, th_vf2 *ic);

// entity
th_quad th_ent_transform(th_ent *e);
void th_ent_draw(th_ent *o, th_rect *camera);
void th_ent_getcoll(th_ent *e, th_ent **scene, uu count, uu *collC,
	uu maxColls, th_coll *colls);

// colour
uint32_t th_color_rgb(float r, float g, float b, float a);
uint32_t th_color_hsv2rgb(float h, float s, float v, float a);

// font
th_font *th_font_load(char *path, double size, uint32_t filter);
void th_font_draw(th_font *font, const char *s, double x, double y, uint32_t color, double scale);
th_vf2 th_font_measure(th_font *font, const char *s);
void th_font_deinit();

// gl
void th_gl_init();
GLuint th_gl_compile_shader(const char **src, GLenum type);
GLuint th_gl_create_prog(const char *vert_src, const char *frag_src, const char **attribs, int nattribs);
void th_gl_free_prog(GLuint prog);

// image
th_image *th_image_alloc();
th_image *th_load_image(char *path);
void th_image_free(th_image *img);
void th_image_from_data(th_image *img, uint32_t *data, th_vf2 dm);

unsigned int th_gen_texture(uint32_t *data, th_vf2 dm, unsigned filter);
void th_blit_tex(th_image *img, th_quad q, uint32_t color);
void _th_rdimg(th_image *img, unsigned char *data);

void th_image_set_filter(th_image *img, int filter);
void th_image_update_data(th_image *img, uint32_t *data, th_vf2 dm);

void th_image_init();
void th_image_deinit();
void th_image_flush();
int th_image_compile_shader(char *frag, char *vert);

// input
void th_input_key(int keycode, int bDown);
void th_input_cycle();

// light
void th_lightmask_clear(th_lightmask *d);
void th_lightmask_draw(th_lightmask *d, th_rect *cam);
void th_spotlight_stamp(th_spotlight *l, th_lightmask *d);
void _th_lightmask_stamp_point(th_lightmask *d, int x, int y, uint32_t color);

// misc
float th_get_scaling(int w, int h, int camw, int camh);
void th_error(char *text, ...);

// particles
void th_particles_draw(th_particles *p, th_rect cam, int t);

// quad/transform
th_vf2 th_quad_min(th_quad q);
void th_transform_rect(th_quad *q, th_transform t, th_rect r);

// vector
void th_rotate_point(th_vf2 *p, th_vf2 o, fu rot);
void th_vector_normalize(float *x, float *y);

// raycast
void th_ray_getcoll(th_ray *ra, th_coll *colls, int maxColls,
                    int *collCount, th_ent **scene, int sceneLen);

// shader
int th_shader_compile(
	char *vertf, char *fragf, char *vertb, char *fragb,
	const char **verta, int vertac);
void th_shader_deinit();

// tilemap
void th_tmap_draw(th_tmap *t, th_rect *cam);
void th_tmap_autotile(uu *tgt, uu *src, uu w, uu h, uu *tiles, uu limiter);

// tophat
th_font *th_get_font(uu index);
th_shader *th_get_shader(uu index);

th_font *th_get_font_err(uu index);
th_shader *th_get_shader_err(uu index);

th_font *th_alloc_font();
th_shader *th_alloc_shader();

// utf8
size_t th_utf8_decode(uint32_t *out, const char *s);

// window
void th_window_setup(char *name, int w, int h);
void th_window_get_dimensions(int *w, int *h);
int th_window_handle();
void th_window_swap_buffers();
void th_window_clear_frame();
void th_window_begin_scissor(int x, int y, size_t w, size_t h);
void th_window_end_scissor();

#endif
