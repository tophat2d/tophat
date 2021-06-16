#ifndef PARTICLES_H
#define PARTICLES_H

#include <stdbool.h>

typedef struct {
	int start_time;
	int seed;
} _th_particle;

typedef struct {
	int px, py;
	int seed;
	
	int angle_min, angle_max;
	
	int lifetime;
	bool random_lifetime;
	int lifetime_randomness_max;

	double velocity;
	bool random_velocity;
	double velocity_randomness_max;

	double size;
	bool random_size;
	double size_randomness_max;
	//double rotation; TODO
	
	_th_particle *particles;
	int particle_c;
} th_particles;

void th_particles_draw(th_particles *p, int t);

#endif
