#include <SupergoonSound/gnpch.h>
#include <SupergoonSound/include/sound.h>
#include <SupergoonSound/sound/alhelpers.h>
#include <SupergoonSound/sound/openal.h>

int gsInitializeSound(void) {
	return InitializeAl();
}
gsBgm *gsLoadBgm(const char *filename_suffix) {
	gsBgm *bgm = calloc(1, sizeof(*bgm));
	// We need to add one here, since strlen and len do not include their null terminator, and we need that in our string and we are going to combine things.
	size_t name_length = strlen(filename_suffix) + 1;
	char *full_name = malloc(name_length * sizeof(char));
	snprintf(full_name, name_length, "%s", filename_suffix);
	bgm->bgm_name = full_name;
	return bgm;
}

gsBgm *gsLoadBgmWithLoopPoints(const char *filename_suffix, float loop_begin, float loop_end) {
	gsBgm *bgm = gsLoadBgm(filename_suffix);
	bgm->loop_begin = loop_begin;
	bgm->loop_end = loop_end;
	return bgm;
}

int gsPreLoadBgm(gsBgm *bgm, int background) {
	if (!bgm) {
		fprintf(stderr, "Trying to preload a invalid bgm\n");
		return false;
	}
	if (background) {
		PreBakeBackgroundBgm(bgm->bgm_name);
	} else {
		PreBakeBgm(bgm->bgm_name);
	}
	return true;
}

gsSfx *gsNewSfx(const char *filename) {
	gsSfx *sfx = malloc(sizeof(*sfx));
	size_t name_length = strlen(filename) + 1;
	char *full_name = malloc(name_length * sizeof(char));
	snprintf(full_name, name_length, "%s", filename);
	sfx->sfx_name = full_name;
	sfx->loaded_sfx = NULL;
	return sfx;
}

int gsPlayBgm(float volume) {
	return PlayBgmAl(volume);
}

int gsPlayBackgroundBgm(float volume) {
	return PlayBgmBackgroundAl(volume);
}

int gsStopBgm(void) {
	return StopBgmAl();
}

int gsStopBackgroundBgm(void) {
	return StopBackgroundBgmAl();
}
int gsPauseBgm(void) {
	return PauseBgmAl();
}
int gsUnPauseBgm(void) {
	return UnpauseBgmAl();
}

int gsPlaySfxOneShot(gsSfx *sfx, float volume) {
	if (!sfx->loaded_sfx) {
		sfx->loaded_sfx = LoadSfxFileAl(sfx->sfx_name);
	}
	PlaySfxAl(sfx->loaded_sfx, volume);
	return 1;
}

int gsLoadSfx(gsSfx *sfx) {
	if (!sfx->loaded_sfx) {
		sfx->loaded_sfx = LoadSfxFileAl(sfx->sfx_name);
	}
	return (sfx->loaded_sfx != NULL) ? 1 : 0;
}

int gsUnloadSfx(gsSfx *sfx) {
	if (sfx->loaded_sfx) {
		CloseSfxFileAl(sfx->loaded_sfx);
	}
	if (sfx) {
		free(sfx->sfx_name);
		sfx->sfx_name = NULL;
		free(sfx);
		sfx = NULL;
	}
	return (sfx == NULL) ? 1 : 0;
}

void gsUpdateSound(void) {
	UpdateAl();
}
void gsSetPlayerLoops(int loop) {
	SetPlayerLoops(loop);
}
