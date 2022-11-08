// NOTE(skejeton): For OpenGL API that may have different rules for including in different systems.
//                 On windows you need to include windows.h before gl.h!
#ifdef _WIN32
#include <windows.h>
#include <glad.h>
#else
#include <chew.h>
#endif
#include <GL/gl.h>
