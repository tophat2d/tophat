#ifndef MISC_H
#define MISC_H

float th_get_scaling(int w, int h, int camw, int camh);
void th_error(char *text, ...);
void th_rotate_point(float *x, float *y, float cx, float cy, float rot);

#endif
