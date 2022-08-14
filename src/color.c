// Color utilities.

#include "tophat.h"

uint32_t th_color_rgb(float r, float g, float b, float a) {
  return ((int)(r * 255) & 0xFF) << 24 |
         ((int)(g * 255) & 0xFF) << 16 |
         ((int)(b * 255) & 0xFF) << 8 |
         ((int)(a * 255) & 0xFF) << 0;
}

// This function separates hue in 6 partitions:
//   red, yellow, green, cyan, blue, magenta
//   then we basically do a lerp between these partitions..
//   e.g. red to yellow, yellow to green, etc.. this is what the switch case does.
//   probably not very efficient...
uint32_t th_color_hsv2rgb(float h, float s, float v, float a) {
  // NOTE(skejeton): negative fix
  if (h < 0) {
    h = (int)(-h)+h+1;
  }

  float r = 0, g = 0, b = 0, z, p, u, l, d;

  int x = (int)(h * 6);
  d = (1.0-s);
  z = (h*6)-x;
  p = (z*s+d)*v;
  u = ((1.0-z)*s+d)*v;
  l = d*v;

  switch (x % 6) {
    case 0: r = v; g = p; b = l; break;
    case 1: g = v; r = u; b = l; break;
    case 2: g = v; b = p; r = l; break;
    case 3: b = v; g = u; r = l; break;
    case 4: b = v; r = p; g = l; break;
    case 5: r = v; b = u; g = l; break;
  }

  return th_color_rgb(r, g, b, a);
}




