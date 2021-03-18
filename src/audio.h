#ifndef AUDIO_H
#define AUDIO_H

#include "../lib/miniaudio/miniaudio.h"

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

void auinit();

void audeinit();

ma_decoder *auload(char *path);

void auplay(ma_decoder *decoder);

#endif
