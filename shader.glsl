@vs th_vs
in vec2 pos;
in vec2 uv0;
in vec4 color0;
out vec2 uv;
out vec4 color;

void main() {
  vec2 p = pos;
  p *= 2;
  p.y *= -1;
  gl_Position = vec4(p+vec2(-1, 1), -0.1, 1);
  uv = uv0;
  color = color0;
}
@end

@fs th_fs
uniform sampler2D tex;
layout (location = 0) out vec4 frag_color;
in vec2 uv;
in vec4 color;

void main() {
  frag_color = color * texture(tex, uv);
}
@end

@program th th_vs th_fs
