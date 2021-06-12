#ifndef MISC_H
#define MISC_H

float th_get_scaling(int w, int h, int camw, int camh);
void th_error(char *text, ...);
void th_rotate_point(float *x, float *y, float cx, float cy, float rot);
void th_vector_normalize(float x1, float y1, float x2, float y2, float *rx, float *ry);

#endif
