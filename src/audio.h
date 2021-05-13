#ifndef AUDIO_H
#define AUDIO_H

#include "../lib/miniaudio.h"

typedef struct {
    ma_decoder decoder;
    int playing;
    float volume;
    int looping;
} sound;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

void auinit();

void audeinit();

sound *auload(char *path);

#endif
