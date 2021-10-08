#ifndef TOPHAT_H
#define TOPHAT_H

#include <stdbool.h>
#include <miniaudio.h>
#include <CNFG.h>
#include <stb_truetype.h>

#define MAX_SOUNDS 512
#define MAX_IMAGES 1024
#define MAX_FONTS 128

typedef float fu;
typedef unsigned short uu;
typedef short iu;

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
	th_vf2 pos, scale, origin;
	fu rot;
} th_transform;

typedef struct {
    ma_decoder decoder;
    int playing;
    float volume;
    int looping;
} th_sound;

typedef struct {
	th_vf2 dm;
	uu channels;
	uint32_t *data;
	unsigned int gltexture;
	uu filter;
} th_image;

#pragma pack(push, 1)
typedef struct {
	int x;
	int y;
	int *v;
	int w;
	int h;
	int vc;
} th_poly;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
	th_rect rect;
	th_image *img;
	th_transform t;
	uint32_t color;
} th_ent;
#pragma pack(pop)

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

#pragma pack(push, 1)
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

	uint32_t *colors;
	uu color_c;

	_th_particle *particles;
	uu particle_c;
} th_particles;
#pragma pack(pop)

typedef struct {
	th_vf2 pos;
	fu l;
	fu r;
} th_ray;

#pragma pack(push, 1)
typedef struct {
	th_image **tiles;
	th_vf2 pos;
	uu w, h;
	uu *cells;
	char *collmask;
	fu cellsize;
	uu scaletype;
} th_tmap;
#pragma pack(pop)

typedef struct {
	stbtt_fontinfo *info;
} th_font;

// struct holding all tophat's global variables.
typedef struct {
	char respath[1024];
	fu scaling;
	void *umka;

	uu pressed[255];
	uu just_pressed[255];
	th_vf2 mouse;

	th_sound *sounds[MAX_SOUNDS];
	uu sound_count;
} th_global;

// audio
void th_audio_init();
void th_audio_deinit();
void th_sound_load(char *path);

// entity
th_quad th_ent_transform(th_ent *e);
void th_ent_draw(th_ent *o, th_rect *camera);
iu th_ent_getcoll(th_ent *e, th_ent **scene, uu count, th_vf2 *ic);

// image
th_image *th_load_image(char *path);
void th_free_image(th_image *img);
void th_image_from_data(th_image *img, uint32_t *data, th_vf2 dm);
void th_image_set_filter(th_image *img, int filter);
unsigned int th_gen_texture(uint32_t *data, th_vf2 dm, unsigned filter);
void th_blit_tex(unsigned int tex, th_quad q);
void th_image_flipv(th_image *img);
void th_image_fliph(th_image *img);
void th_image_crop(th_image *img, th_vf2 tl, th_vf2 br);

// light
void th_lightmask_clear(th_lightmask *d);
void th_lightmask_draw(th_lightmask *d, th_rect *cam);
void th_spotlight_stamp(th_spotlight *l, th_lightmask *d);

// misc
float th_get_scaling(int w, int h, int camw, int camh);
void th_error(char *text, ...);
void th_rotate_point(th_vf2 *p, th_vf2 o, fu rot);
void th_vector_normalize(float *x, float *y);

// particles
void th_particles_draw(th_particles *p, th_rect cam, int t);

// raycast
int th_ray_getcoll(th_ray *ra, th_ent **scene, int count, th_vf2 *ic);

// tilemap
void th_tmap_draw(th_tmap *t, th_rect *cam);

// font
void th_font_load(th_font *out, char *path);
void th_str_to_img(
	th_image *out, th_font *font,
	uint32_t *runes, uu runec,
	fu scale, uint32_t color,
	th_vf2 spacing);

th_vf2 th_quad_min(th_quad q);

//// "unexported" functions

// audio
void _th_audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

// collisions
int th_line_to_line(th_vf2 b1, th_vf2 e1, th_vf2 b2, th_vf2 e2, th_vf2 *ic);
uu th_point_to_quad(th_vf2 p, th_quad *q, th_vf2 *ic);
uu th_ent_to_ent(th_ent *e1, th_ent *e2, th_vf2 *ic);
uu th_line_to_quad(th_vf2 b, th_vf2 e, th_quad *q, th_vf2 *ic);
uu _th_coll_on_tilemap(th_ent *e, th_tmap *t, uu *vert, th_vf2 *tc);
bool th_ray_to_tilemap(th_ray *ra, th_tmap *t, th_vf2 *ic);

// image
void _th_rdimg(th_image *img, unsigned char *data);

// light
void _th_lightmask_stamp_point(th_lightmask *d, int x, int y, uint32_t color);

#endif
