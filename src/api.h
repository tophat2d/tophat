#ifdef C_ONLY
#ifndef API_H
#define API_H

typedef uint32_t th_color

void th_init(char *name, int w, int h);
int th_cycle(th_rect cam, th_color bgcolor);
int th_input_is_pressed(char key);
int th_input_is_just_pressed(char key);

#endif // (API_H)
#endif // (C_ONLY)
