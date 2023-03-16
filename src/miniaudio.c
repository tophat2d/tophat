#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_COREAUDIO

#ifndef __EMSCRIPTEN__
#define MA_NO_WEBAUDIO
#endif

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
