#ifndef TOPHAT_H
#define TOPHAT_H

#include <stdbool.h>
#include <miniaudio.h>
#include <CNFG.h>
#include <stb_truetype.h>

typedef float fu;
typedef unsigned short uu;
typedef short iu;

typedef union {
	struct {fu w, h;};
	struct {fu x, y;};
} th_vf2;

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
	th_poly p;
	th_image *img;
	th_vf2 scale;
	fu rot;
	uint32_t color;
	uu id;
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

typedef struct {
	uint64_t start_time;
	int seed;
} _th_particle;

#pragma pack(push, 1)
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

typedef struct {
	fu x, y, w, h;
} th_rect;

typedef struct {
	th_image **tiles;
	th_vf2 pos;
	uu w, h;
	uu *cells;
	char *collmask;
	fu cellsize;
	uu scaletype;
} th_tmap;

typedef struct {
	stbtt_fontinfo *info;
} th_font;

// audio
void th_audio_init();
void th_audio_deinit();
void th_sound_load(th_sound *s, char *path);

// entity
void th_ent_draw(th_ent *o, th_rect *camera);
int th_ent_getcoll(th_ent *e, th_ent *scene, int count, int *ix, int *iy);

// image
th_image *th_load_image(char *path);
void th_free_image(th_image *img);
void th_image_from_data(th_image *img, uint32_t *data, th_vf2 dm);
void th_image_set_filter(th_image *img, int filter);
unsigned int th_gen_texture(uint32_t *data, th_vf2 dm, unsigned filter);
void th_blit_tex(unsigned int tex, int x, int y, int w, int h, float rot);
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
void th_rotate_point(float *x, float *y, float cx, float cy, float rot);
void th_vector_normalize(float *x, float *y);

// particles
void th_particles_draw(th_particles *p, th_rect cam, int t);

// raycast
int th_ray_getcoll(th_ray *ra, th_ent *scene, int count, int *ix, int *iy);

// tilemap
void th_tmap_draw(th_tmap *t, th_rect *cam);

// font
void th_font_load(th_font *out, char *path);
void th_str_to_img(
	th_image *out, th_font *font,
	uint32_t *runes, uu runec,
	fu scale, uint32_t color,
	th_vf2 spacing);

//// "unexported" functions

// audio
void _th_audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

// collisions
int _th_poly_to_poly(th_poly *p1, th_poly *p2, int *ix, int *iy);
int _th_poly_to_line(th_poly *a, int sx, int sy, int ex, int ey, int *ix, int *iy);
int _th_line_to_line(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int *ix, int *iy);
int _th_poly_to_point(th_poly *a, int px, int py, int *ix, int *iy);
bool th_ray_to_tilemap(th_ray *ra, th_tmap *t, int *ix, int *iy);
bool _th_coll_on_tilemap(th_poly *p, th_tmap *t, int *rx, int *ry, int *rtx, int *rty);

// image
void _th_rdimg(th_image *img, unsigned char *data);

// input
void _th_input_init();

// light
void _th_lightmask_stamp_point(th_lightmask *d, int x, int y, uint32_t color);

// polygon
RDPoint *_th_poly_to_rdpoint(th_poly *p, int camx, int camy);

#endif
