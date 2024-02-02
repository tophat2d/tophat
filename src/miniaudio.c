#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_COREAUDIO

#ifndef __EMSCRIPTEN__
#define MA_NO_WEBAUDIO
#endif

#define STB_VORBIS_HEADER_ONLY
#include <extras/stb_vorbis.c>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
