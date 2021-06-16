#include <math.h>
#include <stdlib.h>

#include "particles.h"
#include "../lib/rawdraw/CNFG.h"

#define FRAND (double)rand()/0x7FFFFFFF

void th_particles_draw(th_particles *p, int t) {
	for (int i=0; i < p->particle_c; i++) {
		srand(p->particles[i].seed);

		double direction = (rand()%(p->angle_max - p->angle_min) + p->angle_min) * M_PI / 180;
		
		double vx = p->velocity * cos(direction) + FRAND/10;// + frand()/p->velocity_randomness_max * p->random_velocity;
		double vy = p->velocity * sin(direction) + FRAND/10;// + frand()/p->velocity_randomness_max * p->random_velocity;

		int px = p->px + vx * (t - p->particles[i].start_time);
		int py = p->py + vy * (t - p->particles[i].start_time);

		double size = p->size;// + frand()/p->size_rand


		CNFGColor(0xff);
		CNFGTackRectangle(px, py, px + size, py + size); // TODO camera

		if (t - p->particles[i].start_time >= p->lifetime) {
			p->particles[i].start_time = t;
			p->particles[i].seed = rand();
			//printf("time: %d\n", t);
		}
	}
}

