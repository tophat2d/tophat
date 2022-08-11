// audio file
// inpired by https://github.com/drmargarido's sound module
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "tophat.h"

#include <miniaudio.h>
#define ma_countof(x) (sizeof(x) / sizeof(x[0]))

#define SAMPLE_FORMAT ma_format_f32
#define CHANNEL_COUNT 2
#define SAMPLE_RATE 48000

ma_device_config auconf;
ma_device audev;
ma_decoder_config decodercfg;

extern th_global thg;

ma_uint32 __read_and_mix_pcm_frames_f32(ma_decoder* pDecoder, float* pOutputF32, ma_uint32 frameCount, float volume) {
	float temp[4096];
	ma_uint32 tempCapInFrames = ma_countof(temp) / CHANNEL_COUNT;
	ma_uint32 totalFramesRead = 0;

	while (totalFramesRead < frameCount) {
		ma_uint32 iSample;
		ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
		ma_uint32 framesToReadThisIteration = tempCapInFrames;
		if (framesToReadThisIteration > totalFramesRemaining) {
			framesToReadThisIteration = totalFramesRemaining;
		}


		ma_uint64 framesReadThisIteration;
		ma_decoder_read_pcm_frames(pDecoder, temp, framesToReadThisIteration, &framesReadThisIteration);
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

void _th_audio_data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
	th_playback_item *prev = NULL;

	for (th_playback_item *pbi = thg.playbacks; pbi; pbi = pbi->next) {
		th_playback *pk = pbi->pk;
		if (pk->paused) continue;

		if (pk->frame >= 0) {
			ma_decoder_seek_to_pcm_frame(pk->decoder, pk->frame);
			pk->frame = -1;
		}

		ma_uint32 framesread = __read_and_mix_pcm_frames_f32(
			pk->decoder,
			(float *)pOutput,
			frameCount,
			pk->volume);

		if (framesread <= 0 || !pk->playing) {
			pk->frame = 0;
			if (pk->looping) continue;

			pk->playing = 0;
			umkaDecRef(thg.umka, pk);
			if (prev)
				prev->next = pbi->next;
			else
				thg.playbacks = pbi->next;

			free(pbi);
			pbi = prev ? prev->next : thg.playbacks;

			if (!pbi) break;
		}
	}
}

void th_audio_init(){
	auconf = ma_device_config_init(ma_device_type_playback);
	auconf.playback.format = SAMPLE_FORMAT;
	auconf.playback.channels = CHANNEL_COUNT;
	auconf.sampleRate = SAMPLE_RATE;
	auconf.dataCallback = _th_audio_data_callback;
	auconf.pUserData = NULL;

	if (ma_device_init(NULL, &auconf, &audev) != MA_SUCCESS) {
		th_error("Failed to open playback device.");
		return;
	}

	if (ma_device_start(&audev) != MA_SUCCESS) {
		th_error("Failed to start playback device.");
		return;
	}

	decodercfg = ma_decoder_config_init(SAMPLE_FORMAT, CHANNEL_COUNT, SAMPLE_RATE);
}

void th_audio_deinit() {
	ma_device_uninit(&audev);

	for (th_playback_item *pbi = thg.playbacks; pbi;) {
		umkaDecRef(thg.umka, pbi->pk);
		th_playback_item *tmp = pbi;

		pbi = pbi->next;

		free(tmp);
	}
}
