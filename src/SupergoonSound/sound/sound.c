#include <SupergoonSound/gnpch.h>
#include <SupergoonSound/sound/alhelpers.h>
#include <SupergoonSound/sound/openal.h>
#include <SupergoonSound/include/sound.h>

/**
 * @brief The prefix that we should add to the config file so that we look in the right location.
 */
static const char *sfx_prefix = "";

int gsInitializeSound(void)
{
    return InitializeAl();
}

Bgm* gsLoadBgm(const char *filename_suffix, float loop_begin, float loop_end)
{
    Bgm *bgm = malloc(sizeof(*bgm));
    // We need to add one here, since strlen and len do not include their null terminator, and we need that in our string and we are going to combine things.
    size_t name_length = strlen(sfx_prefix) + strlen(filename_suffix) + 1;
    char *full_name = malloc(name_length * sizeof(char));
    snprintf(full_name, name_length, "%s%s", sfx_prefix, filename_suffix);
    bgm->bgm_name = full_name;
    bgm->loop_begin = loop_begin;
    bgm->loop_end = loop_end;
    return bgm;
}

int gsPreLoadBgm(Bgm* bgm)
{
    PreBakeBgm(bgm->bgm_name, &bgm->loop_begin, &bgm->loop_end);
    return true;
}

Sfx* gsLoadSfxHelper(const char* filename)
{
        Sfx *sfx = malloc(sizeof(*sfx));
        size_t name_length = strlen(sfx_prefix) + strlen(filename) + 1;
        char *full_name = malloc(name_length * sizeof(char));
        snprintf(full_name, name_length, "%s%s", sfx_prefix, filename);
        sfx->sfx_name = full_name;
        sfx->loaded_sfx = NULL;
        return sfx;
}

int gsPlayBgm(float volume)
{
    return PlayBgmAl(volume);
}

int gsStopBgm(void)
{
    return StopBgmAl();
}
int gsPauseBgm(void)
{
    return PauseBgmAl();
}
int gsUnPauseBgm(void)
{
    return UnpauseBgmAl();
}

int gsPlaySfxOneShot(Sfx* sfx, float volume)
{
    if (!sfx->loaded_sfx)
    {
        sfx->loaded_sfx = LoadSfxFileAl(sfx->sfx_name);
    }
    PlaySfxAl(sfx->loaded_sfx, volume);
    return 1;
}

int gsLoadSfx(Sfx* sfx)
{
    if (!sfx->loaded_sfx)
    {
        sfx->loaded_sfx = LoadSfxFileAl(sfx->sfx_name);
    }
    return (sfx->loaded_sfx != NULL) ? 1 : 0;
}

int gsUnloadSfx(Sfx* sfx)
{
    if (sfx->loaded_sfx)
    {
        CloseSfxFileAl(sfx->loaded_sfx);
    }
    if (sfx)
    {
        free(sfx->sfx_name);
        sfx->sfx_name = NULL;
        free(sfx);
        sfx = NULL;
    }
    return (sfx == NULL) ? 1 : 0;
}

void gsUpdateSound(void)
{
    UpdateAl();
}
