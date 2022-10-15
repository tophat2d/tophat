#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "tophat.h"

#include <umka_api.h>

#include <miniaudio.h>
#define ma_countof(x) (sizeof(x) / sizeof(x[0]))

extern th_global *thg;

int allocd = 0, freed = 0;

static
void sound_free(UmkaStackSlot *p, UmkaStackSlot *r) {
	th_sound *s = (th_sound *)p[0].ptrVal;

	// NOTE(@marekmaskarinec): for some reason ma doesn't free this in my
	// case which results in a memory leak, however produces double with
	// the original sounds. FIXME
	/*ma_resource_manager_data_source_uninit(s->inst.pResourceManagerDataSource);
	ma_free(
		s->inst.pResourceManagerDataSource,
		&s->inst.engineNode.pEngine->allocationCallbacks
	);*/

	ma_sound_uninit(&s->inst);
}

th_sound *th_audio_load(char *path) {
	th_sound *s = umkaAllocData(thg->umka, sizeof(th_sound), sound_free);

	if (ma_sound_init_from_file(
		&thg->audio_engine,
		path,
		MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION,
		NULL,
		NULL,
		&s->inst
	) != MA_SUCCESS) {
		th_error("Could not load sound at path %s.", path);
		return NULL;
	}

	return s;
}

th_sound *th_sound_copy(th_sound *s) {
	th_sound *r = umkaAllocData(thg->umka, sizeof(th_sound), sound_free);
	
	if (ma_sound_init_copy(
		&thg->audio_engine,
		&s->inst,
		MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION,
		NULL,
		&r->inst
	) != MA_SUCCESS) {
		th_error("Could not copy a sound.");
		return NULL;
	}

	return r;
}

void th_audio_init(){
	if (ma_engine_init(NULL, &thg->audio_engine) != MA_SUCCESS) {
		th_error("Failed to create an audio engine.");
		return;
	}
}

void th_audio_deinit() {
	ma_engine_uninit(&thg->audio_engine);
}

