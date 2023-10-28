#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "tophat.h"

#define FRAND() (double)rand() / 0x7FFFFFFF

extern th_global *thg;

static float
interp(float start, float start_val, float end, float end_val, int t)
{
	return (start_val + (t - start) * ((end_val - start_val) / (end - start)));
}

static uint32_t
get_particle_color(th_particles *p, _th_particle pa, int t)
{
	int n = (t - pa.start_time) / (p->lifetime / umkaGetDynArrayLen(&p->colors));
	return p->colors.data[n];
}

static float
get_particle_size(th_particles *p, _th_particle pa, int t)
{
	int x0 = pa.start_time;
	int x1 = pa.start_time + p->lifetime;
	return interp(x0, p->size, x1, p->max_size, t);
}

static int
get_particle_rotation(th_particles *p, _th_particle pa, int t)
{
	return interp(0, p->rotation, p->lifetime, p->max_rotation, t - pa.start_time);
}

void
th_particles_draw(th_particles *p, int t)
{
	p->active = false;

	for (int i = 0; i < umkaGetDynArrayLen(&p->particles); i++) {
		p->active = true;
		if (t < p->particles.data[i].start_time)
			continue;

		srand(p->particles.data[i].seed);

		if ((p->angle.y - p->angle.x) + p->angle.x == 0)
			p->angle.x++;
		float direction =
		    (rand() % (long)(p->angle.y - p->angle.x + 1) + p->angle.x) * PI / 180;

		fu vx = p->velocity * cos(direction);
		fu vy = p->velocity * sin(direction);
		if (p->velocity_randomness != 0) {
			vx += FRAND() * p->velocity * p->velocity_randomness;
			vy += FRAND() * p->velocity * p->velocity_randomness;
		}

		vx *= p->gravity.x;
		vy *= p->gravity.y;

		if (!p->dm.w)
			p->dm.w = 1;
		if (!p->dm.h)
			p->dm.h = 1;
		fu px =
		    p->pos.x + rand() % (iu)p->dm.w + vx * (t - p->particles.data[i].start_time);
		fu py =
		    p->pos.y + rand() % (iu)p->dm.h + vy * (t - p->particles.data[i].start_time);

		fu size = p->size;
		if (p->size != p->max_size)
			size = get_particle_size(p, p->particles.data[i], t);

		size += FRAND() * p->size * p->size_randomness;

		uint32_t col = 0xff;
		if (umkaGetDynArrayLen(&p->colors) > 0)
			col = get_particle_color(p, p->particles.data[i], t);

		if (p->max_rotation) {
			int rot = p->rotation;
			if (p->rotation != p->max_rotation)
				rot = get_particle_rotation(p, p->particles.data[i], t);

			rot += FRAND() * p->size * p->rotation_randomness;

			th_vf2 p[4] = {{.x = px, .y = py}, {.x = px + size, .y = py},
			    {.x = px + size, .y = py + size}, {.x = px, .y = py + size}};
			for (int i = 0; i < 4; i++) {
				th_rotate_point(
				    &p[i], (th_vf2){{px + size / 2, py + size / 2}}, rot);
				p[i].x = p[i].x * thg->scaling;
				p[i].y = p[i].y * thg->scaling;
			}

			th_canvas_triangle(col, p[0], p[1], p[2]);
			th_canvas_triangle(col, p[0], p[2], p[3]);
		} else { // optimize drawing without rotations
			fu x = px;
			fu y = py;
			fu w = size;
			fu h = size;

			th_canvas_rect(col, (th_rect){x, y, w, h});
		}

		int lt = p->lifetime + FRAND() * p->lifetime * p->lifetime_randomness;
		if (t - p->particles.data[i].start_time >= lt) {
			if (p->repeat) {
				p->particles.data[i].start_time = t - rand() % (p->lifetime / 4);
				p->particles.data[i].seed = rand();
			} else {
				p->particles.data[i].start_time = -1;
				p->active = false;
			}
		}
	}
}
