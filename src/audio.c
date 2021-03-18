// vim: filetype=c

#define MINIAUDIO_IMPLEMENTATION
#include "../lib/miniaudio/miniaudio.h"

#include "audio.h"

ma_device audev;
ma_device_config auconf;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
		if (pDecoder == NULL) {
	    return;
		}
	
		ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);
	
		(void)pInput;
}

void auinit() {
	/*ma_decoder *decoder;
	decoder = auload("test.wav");

	auconf.playback.format = decoder->outputFormat;
	auconf.playback.channels = decoder->outputChannels;
	auconf.sampleRate = decoder->outputSampleRate;
	auconf.dataCallback = data_callback;
	auconf.pUserData = decoder;i*/

}

void audeinit() {
	ma_device_uninit(&audev);
}

ma_decoder *auload(char *path) {
	ma_decoder *decoder;
	decoder = malloc(sizeof(ma_decoder));

	ma_result result = ma_decoder_init_file(path, NULL, decoder);
	if (result != MA_SUCCESS) {
		printf("could not load file");
		return NULL;
	}

	return decoder;
}

void auplay(ma_decoder *decoder) {
	auconf = ma_device_config_init(ma_device_type_playback);
	auconf.playback.format = decoder->outputFormat;
	auconf.playback.channels = decoder->outputChannels;
	auconf.sampleRate = decoder->outputSampleRate;
	auconf.dataCallback = data_callback;
	auconf.pUserData = decoder;

	printf("playing sound\n");

	if (ma_device_init(NULL, &auconf, &audev) != MA_SUCCESS) {
		printf("could not open sound device\n");
		return;
	}

	if (ma_device_start(&audev) != MA_SUCCESS) {
		printf("failed to start device\n");
		return;
	}
}
