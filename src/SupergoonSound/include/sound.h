/**
 * @file sound.h
 * @brief High level wrapper over the openal.h commands to easily play BGM and SFX
 * @author Kevin Blanchard
 * @version 0.1
 * @date 2022-09-17
 */

#pragma once
typedef struct Sg_Loaded_Sfx Sg_Loaded_Sfx;
#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Structure to hold a bgm with it's loop points.
     */
    typedef struct gsBgm
    {
        char *bgm_name;
        double loop_begin;
        double loop_end;

    } Bgm;

    /**
     * @brief Holds a sfx name and the loaded file if it is loaded.
     */
    typedef struct gsSfx
    {
        char *sfx_name;
        Sg_Loaded_Sfx *loaded_sfx;
    } Sfx;

    Bgm *gsLoadBgm(const char *filename, float loop_begin, float loop_end);
    int gsPreLoadBgm(Bgm *bgm);
    Sfx *gsLoadSfxHelper(const char *filename);
    /**
     * @brief Load the Sound backend, this must be called before any other functions are available.
     *
     * @return 1 if successful, 0 if failure.
     */
    int gsInitializeSound(void);
    /**
     * @brief Play a specific BGM.  It will loop continuously until you call the Stop function on it.  Its loop points and number are determined by the config file.
     *
     * @param bgm_number The current bgm number to play.
     * @param volume The volume we want to set this to.  1 is regular volume.
     *
     * @return 1 if Successful, 0 if failed to start.
     */
    int gsPlayBgm(float volume);
    /**
     * @brief Stops a playing bgm.  If stop_at_end is true, then it will stop playing at the end of the song.
     *
     * @param stop_at_end If sent 1, this will stop at the end of the song instead of now.
     *
     * @return 1 if successful, 0 if failed.
     */
    int gsStopBgm(void);
    int gsPauseBgm(void);
    int gsUnPauseBgm(void);
    /**
     * @brief Plays a Sound effect once in its own buffer.  There is only a total of 10 buffers available for playing at a time. If the sound is not loaded, will load the sound
     *
     * @param sfx_number The Sound effect to play
     *
     * @return 1 if successful, 0 if failed to start
     */
    int gsPlaySfxOneShot(Sfx *sfx_number, float volume);
    /**
     * @brief Preloads a sfx sound.
     *
     * @param sfx_number the sfx number to load.
     *
     * @return Returns 1 if it was loaded or already loaded, and 0 if load failed.
     */
    int gsLoadSfx(Sfx *sfx_number);
    /**
     * @brief Unloads a loaded sound
     *
     * @param sfx_number The number sfx that we should unload
     *
     * @return 1 if it was unloaded or wasn't loaded already, and 0 if it was not null
     */
    int gsUnloadSfx(Sfx *);
    /**
     * @brief This should be called every frame.  Updates the BGM sound and such.
     */
    void gsUpdateSound(void);
    /**
     * @brief Closes openal and destroys all bgm and sfx.
     */
    void gsCloseSound(void);
    void gsSetPlayerLoops(int loop);

#ifdef __cplusplus
}
#endif