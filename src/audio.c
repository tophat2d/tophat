// audio file
// inpired by https://github.com/drmargarido's sound module

#include <stdint.h>

#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_COREAUDIO
#define MA_NO_WEBAUDIO
#define MINIAUDIO_IMPLEMENTATION
#include "../lib/miniaudio.h"

#include "audio.h"
#include "misc.h"

#define SAMPLE_FORMAT ma_format_f32
#define CHANNEL_COUNT 2
#define SAMPLE_RATE 48000

ma_device_config auconf;
ma_device audev;
sound **sounds;
int soundcount;

extern char *respath;

ma_uint32 __read_and_mix_pcm_frames_f32(ma_decoder* pDecoder, float* pOutputF32, ma_uint32 frameCount, float volume) {
    float temp[4096];
    ma_uint32 tempCapInFrames = ma_countof(temp) / CHANNEL_COUNT;
    ma_uint32 totalFramesRead = 0;

    while (totalFramesRead < frameCount) {
        ma_uint32 iSample;
        ma_uint32 framesReadThisIteration;
        ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
        ma_uint32 framesToReadThisIteration = tempCapInFrames;
        if (framesToReadThisIteration > totalFramesRemaining) {
            framesToReadThisIteration = totalFramesRemaining;
        }

        framesReadThisIteration = (ma_uint32)ma_decoder_read_pcm_frames(pDecoder, temp, framesToReadThisIteration);
        if (framesReadThisIteration == 0) {
            break;
        }

        /* Mix the frames together. */
        for (iSample = 0; iSample < framesReadThisIteration*CHANNEL_COUNT; ++iSample) {
            pOutputF32[totalFramesRead*CHANNEL_COUNT + iSample] += temp[iSample] * volume;
        }

        totalFramesRead += framesReadThisIteration;

        if (framesReadThisIteration < framesToReadThisIteration) {
            break;  /* Reached EOF. */
        }
    }
    
    return totalFramesRead;
}

void data_callback(ma_device * pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
	for (int i=soundcount-1; i >= 0; i--) {
		if (sounds[0] == NULL)
			continue;

		sound *csound = sounds[i];

		if (csound == NULL)
			continue;

		if (!csound->playing)
			continue;

		ma_uint32 framesread = __read_and_mix_pcm_frames_f32(&csound->decoder, (float *)pOutput, frameCount, csound->volume);
										
		if (framesread <= 0) {
			ma_decoder_seek_to_pcm_frame(&csound->decoder, 0);
			if (csound->looping)
				continue;
			csound->playing = 0;
		}
	}
}

void auinit(){
	auconf = ma_device_config_init(ma_device_type_playback);
	auconf.playback.format = SAMPLE_FORMAT;
	auconf.playback.channels = CHANNEL_COUNT;
	auconf.sampleRate = SAMPLE_RATE;
	auconf.dataCallback = data_callback;
	auconf.pUserData = NULL;

	if (ma_device_init(NULL, &auconf, &audev) != MA_SUCCESS) {
		errprint("Failed to open playback device.");
		return;
	}

	if (ma_device_start(&audev) != MA_SUCCESS) {
		errprint("Failed to start playback device.");
		return;
	}
}

void audeinit() {
	ma_device_uninit(&audev);

	for (int i=soundcount - 1; i >= 0; i--) {
		ma_decoder_uninit(&sounds[i]->decoder);

		free(sounds[i]);
	}
}

sound *auload(char *path) {
	sound *s;
	s = malloc(sizeof(sound));

	ma_decoder_config decodercfg;
	decodercfg = ma_decoder_config_init(SAMPLE_FORMAT, CHANNEL_COUNT, SAMPLE_RATE);

	ma_result res;
	char cpath[512];
	strcpy(cpath, respath);
	strcat(cpath, path);
	res = ma_decoder_init_file(cpath, &decodercfg, &s->decoder);
	if (res != MA_SUCCESS) {
		char buff[256];
		sprintf("failed to load sound at %s", path);
		errprint(buff);
		return NULL;
	}

	s->playing = 0;
	s->volume = 100;
	s->looping = 0;

	return s;
}
