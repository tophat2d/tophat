#ifndef PARTICLES_H
#define PARTICLES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	int start_time;
	int seed;
} _th_particle;

typedef struct {
	int px, py;
	int w, h;
	int seed;
	
	int angle_min, angle_max;
	
	int lifetime;
	int lifetime_randomness;

	double velocity;
	double velocity_randomness;

	double size;
	double size_randomness;
	//double rotation; TODO
	
	uint32_t *colors;
	int color_c;

	_th_particle *particles;
	int particle_c;
} th_particles;

void th_particles_draw(th_particles *p, int t);

#endif
