#ifndef AUDIO_H
#define AUDIO_H

#include "../lib/miniaudio.h"

typedef struct {
    ma_decoder decoder;
    int playing;
    float volume;
    int looping;
} th_sound;

void _th_audio_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

void th_audio_init();

void th_audio_deinit();

void th_sound_load(th_sound *s, char *path);

#endif
