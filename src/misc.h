#ifndef MISC_H
#define MISC_H

void slp(int t);

int getscaling(int w, int h, int camw, int camh);

void errprint(char *text);

void rotatepoint(float *x, float *y, float cx, float cy, float rot);

#endif
