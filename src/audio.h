#ifndef AUDIO_H
#define AUDIO_H

#include "../lib/miniaudio/miniaudio.h"

typedef struct {
    uint16_t id;
    ma_decoder decoder;
    int deleted;
    int playing;
    float volume;
    int looping;
} sound;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

void auinit();

void audeinit();

sound *auload(char *path);

#endif
