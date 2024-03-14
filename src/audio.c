#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tophat.h"

#include <umka_api.h>

#include <miniaudio.h>
#define ma_countof(x) (sizeof(x) / sizeof(x[0]))

extern th_global *thg;

int allocd = 0, freed = 0;

static void
sound_free(UmkaStackSlot *p, UmkaStackSlot *r)
{
	th_sound *s = (th_sound *)p[0].ptrVal;

	ma_sound_uninit(&s->inst);

	// NOTE(~mrms): this causes occasional double-frees
	/*if (!s->copied) return;
	ma_resource_manager_data_source_uninit(s->inst.pResourceManagerDataSource);
	ma_free(
		s->inst.pResourceManagerDataSource,
		&s->inst.engineNode.pEngine->allocationCallbacks
	);*/
}

th_err
th_audio_load(th_sound **out, char *path, uint32_t flags)
{
	char regularized_path[4096];
	th_regularize_path(path, "./", regularized_path, sizeof regularized_path);
	path = regularized_path;

	th_sound *s = umkaAllocData(thg->umka, sizeof(th_sound), sound_free);
	s->copied = 0;

	if (ma_sound_init_from_file(&thg->audio_engine, path,
		MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION | flags, NULL, NULL,
		&s->inst) != MA_SUCCESS) {
		return th_err_io;
	}

	*out = s;
	return 0;
}

th_err
th_sound_copy(th_sound **out, th_sound *s)
{
	th_sound *r = umkaAllocData(thg->umka, sizeof(th_sound), sound_free);
	r->copied = true;

	if (ma_sound_init_copy(&thg->audio_engine, &s->inst,
		MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION, NULL,
		&r->inst) != MA_SUCCESS) {
		return 1;
	}

	*out = r;
	return 0;
}

void
th_audio_init()
{
	if (ma_engine_init(NULL, &thg->audio_engine) != MA_SUCCESS) {
		th_error("Failed to create an audio engine.");
		return;
	}
}

void
th_audio_deinit()
{
	ma_engine_uninit(&thg->audio_engine);
}
