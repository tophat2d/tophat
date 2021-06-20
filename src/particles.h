#ifndef PARTICLES_H
#define PARTICLES_H

#include <stdbool.h>
#include <stdint.h>

#include "rect.h"

typedef struct {
	int start_time;
	int seed;
} _th_particle;

#pragma pack(push, 1)
typedef struct {
	int px, py;
	int w, h;
	double gravity_x, gravity_y;
	int seed;
	
	int angle_min, angle_max;
	
	int lifetime;
	double lifetime_randomness;

	double velocity;
	double velocity_randomness;

	double size;
	double size_randomness;
	double max_size;
	
	int rotation;
	int max_rotation;
	double rotation_randomness;

	uint32_t *colors;
	int color_c;

	_th_particle *particles;
	int particle_c;
} th_particles;
#pragma pack(pop)

void th_particles_draw(th_particles *p, th_rect cam, int t);

#endif
