#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "particles.h"
#include "../lib/rawdraw/CNFG.h"

#define FRAND (double)rand()/0x7FFFFFFF

void th_particles_draw(th_particles *p, int t) {
	for (int i=0; i < p->particle_c; i++) {
		srand(p->particles[i].seed);

		double direction = (rand()%(p->angle_max - p->angle_min) + p->angle_min) * M_PI / 180;
		
		double vx = p->velocity * cos(direction);
		double vy = p->velocity * sin(direction);
		if (p->velocity_randomness != 0) {
			vx += FRAND*(p->velocity*p->velocity_randomness);
			vy += FRAND*(p->velocity*p->velocity_randomness);
		}

		int px = p->px + rand()%p->w + vx * (t - p->particles[i].start_time);
		int py = p->py + rand()%p->h + vy * (t - p->particles[i].start_time);

		double size = p->size + FRAND * (p->size * p->size_randomness);

		uint32_t col = 0;

		{
			int n = (t - p->particles[i].start_time) / (p->lifetime / p->color_c);

			//printf("%d\n", n);

			int x0 = p->particles[i].start_time + p->lifetime / p->color_c * n;//(p->lifetime / p->color_c) * n;
			int x = t;
			int y0 = p->colors[n];
			int x1 = p->particles[i].start_time + p->lifetime / p->color_c * (n + 1);
			int y1 = p->colors[n];
			if (n < p->color_c - 1)
				y1 = p->colors[n+1];

			for (int i=0; i < 4; i++) {
				int t0 = y0 & 0xff;
				int t1 = y1 & 0xff;
				col += ((t0 + (t - x0) * ((t1 - t0)/(x1-x0))) << (i * 8));
				y0 >>= 8;
				y1 >>= 8;
			}
			//col = 0xff;
		}

		CNFGColor(col);
		CNFGTackRectangle(px, py, px + size, py + size); // TODO camera

		int lt = p->lifetime + FRAND*(p->lifetime * p->lifetime_randomness);

		if (t - p->particles[i].start_time >= lt) {
			p->particles[i].start_time = t - rand()%(p->lifetime / 4);
			p->particles[i].seed = rand();
			//printf("time: %d\n", t);
		}
	}
}

