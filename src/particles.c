#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "../lib/rawdraw/CNFG.h"

#include "tophat.h"

#define FRAND (double)rand()/0x7FFFFFFF

extern float scaling;

int interp(int start, int start_val, int end, int end_val, int t) {
	return (start_val + (t - start) * ((end_val - start_val)/(end-start)));
}

uint32_t get_particle_color(th_particles *p, _th_particle pa, int t) {
	uint32_t col = 0;

	int n = (t - pa.start_time) / (p->lifetime / p->color_c);

	int x0 = pa.start_time + p->lifetime / p->color_c * n;
	int y0 = p->colors[n];
	int x1 = pa.start_time + p->lifetime / p->color_c * (n + 1);
	int y1 = p->colors[n];
	if (n < p->color_c - 1)
		y1 = p->colors[n+1];

	for (int i=0; i < 4; i++) {
		int t0 = y0 & 0xff;
		int t1 = y1 & 0xff;
		col += (int)round(interp(x0, t0, x1, t1, 1)) << (i * 8);
		y0 >>= 8;
		y1 >>= 8;
	}

	return col;
}

double get_particle_size(th_particles *p, _th_particle pa, int t) {
	int x0 = pa.start_time;
	int x1 = pa.start_time + p->lifetime;
	return interp(x0, p->size * 100, x1, p->max_size * 100, t) / 100.0;
}

int get_particle_rotation(th_particles *p, _th_particle pa, int t) {
	return interp(0, p->rotation * 1000, p->lifetime, p->max_rotation * 1000, t - pa.start_time) / 1000;
}

void th_particles_draw(th_particles *p, th_rect cam, int t) {
	int camx = cam.x - cam.w/2, camy = cam.y - cam.h/2;

	for (int i=0; i < p->particle_c; i++) {
		srand(p->particles[i].seed);

		double direction = (rand()%(p->angle_max - p->angle_min) + p->angle_min) * M_PI / 180;
		
		double vx = p->velocity * cos(direction);
		double vy = p->velocity * sin(direction);
		if (p->velocity_randomness != 0) {
				vx += FRAND*(p->velocity*p->velocity_randomness);
				vy += FRAND*(p->velocity*p->velocity_randomness);
		}

		vx *= p->gravity_x;
		vy *= p->gravity_y;

		int px = p->px + rand()%p->w + vx * (t - p->particles[i].start_time);
		int py = p->py + rand()%p->h + vy * (t - p->particles[i].start_time);

		double size = p->size;
		if (p->size != p->max_size)
			size = get_particle_size(p, p->particles[i], t);

		size += FRAND * (p->size * p->size_randomness);

		uint32_t col = 0;
		if (p->color_c > 0)
			col = get_particle_color(p, p->particles[i], t);

		int rot = p->rotation;
		if (p->rotation != p->max_rotation)
			rot = get_particle_rotation(p, p->particles[i], t);

		rot += FRAND * (p->size * p->rotation_randomness);

		RDPoint points[4] = {{px, py}, {px + size, py}, {px + size, py + size}, {px, py + size}};
		for (int i=0; i < 4; i++) {
			float x = points[i].x;
			float y = points[i].y;
			th_rotate_point(&x, &y, px + size/2, py+size/2, rot);
			points[i].x = (x - camx) * scaling;
			points[i].y = (y - camy) * scaling;
		}

		CNFGColor(col);
		CNFGTackPoly(points, 4); // TODO camera

		int lt = p->lifetime + FRAND*(p->lifetime * p->lifetime_randomness);
		if (t - p->particles[i].start_time >= lt) {
			p->particles[i].start_time = t - rand()%(p->lifetime / 4);
			p->particles[i].seed = rand();
		}
	}
}

