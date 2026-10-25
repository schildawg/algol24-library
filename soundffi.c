/* soundffi.c -- a square wave and a sample mixer, over SDL2's audio device.
 *
 * SDL2 rather than CoreAudio, and the trade is deliberate: CoreAudio is a
 * macOS framework and exists nowhere else, so the unit it backed could only
 * ever have made a noise on one operating system. SDL2 is a dependency this
 * library already has -- `graph` declares it directly -- and it carries audio
 * on macOS, Linux, Windows and Android alike.
 *
 * SDL_INIT_AUDIO opens no window and needs no display server, so a console
 * program that beeps does not acquire a window system by asking for sound. It
 * does acquire SDL, which is the whole of what this change costs.
 *
 * WHAT IT COST: file formats. CoreAudio's ExtAudioFile read WAV, AIFF, CAF,
 * MP3 and M4A through the system's codecs. SDL2 carries one decoder,
 * SDL_LoadWAV, so this reads WAV and nothing else. SDL2_mixer would bring the
 * others back at the price of a second dependency; it is not here.
 *
 * The internal format is float32, mono, 44100, and the device is asked for
 * exactly that with no conversion allowed -- so the mixer below writes
 * straight into SDL's buffer and never resamples. A file is converted to that
 * format once, at load, by SDL's own converter.
 *
 * ALG_SOUND=dummy computes everything and plays nothing, which is what lets
 * the library's tests run without filling the room -- the same bargain
 * SDL_VIDEODRIVER=dummy strikes for `graph`. Under it no device is opened and
 * `alg_sound_play` answers 0, but a file still DECODES: loading asks nothing
 * of the device, which is what keeps `LoadSound` testable with no audio
 * hardware at all.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>

#define RATE      44100
#define MAX_CLIPS 64
#define MAX_VOICE 16
#define MAX_FRAMES (RATE * 600)          /* ten minutes is plenty */

/* ----------------------------------------------------------- the state -- */

typedef struct { float *pcm; int64_t frames; } Clip;
typedef struct { int64_t clip; int64_t at; double gain; int active; } Voice;

static SDL_AudioDeviceID device;
static int               opened = 0;
static int               silent = -1;   /* -1 until the environment is read */

/* The tone. Written from the calling thread and read from the audio thread;
 * both are scalars a store makes visible whole, and a note starting a
 * fraction of a buffer early or late is not something an ear can hear. */
static volatile double tone_hz  = 0.0;
static volatile double tone_amp = 0.18;
static double          tone_ph  = 0.0;

static Clip  clips[MAX_CLIPS];
static Voice voices[MAX_VOICE];

/* Whether to open the device at all. */
static int is_silent (void)
{
    if (silent < 0)
    {
        const char *s = getenv ("ALG_SOUND");

        silent = (s != NULL && strcmp (s, "dummy") == 0) ? 1 : 0;
    }

    return silent;
}

/* Holds the audio thread off the voice table, or does nothing when no device
 * was opened.
 *
 * SDL gives this and CoreAudio had no easy equivalent, so the port closes a
 * real race the old backend only hoped about: `alg_sound_free` deactivated a
 * voice and then freed its samples, with nothing to stop the mixer reading
 * the old pointer in between.
 */
static void hold (void)
{
    if (opened) SDL_LockAudioDevice (device);
}

static void release (void)
{
    if (opened) SDL_UnlockAudioDevice (device);
}

/* ------------------------------------------------------------- the mix -- */

/* Adds the tone and every voice into SDL's buffer. Runs on SDL's audio
 * thread.
 *
 * `len` is bytes and the format is float32 mono, so the frame count is a
 * quarter of it. The device is opened with no conversion allowed, which is
 * what makes that division safe.
 */
static void mix (void *ref, Uint8 *stream, int len)
{
    (void) ref;

    float *out    = (float *) stream;
    int    frames = len / (int) sizeof (float);

    double hz   = tone_hz;
    double amp  = tone_amp;
    double step = hz / (double) RATE;

    for (int i = 0; i < frames; i++)
    {
        double s = 0.0;

        /* A square wave, which is what a PC speaker made and what Sound has
         * always meant. Half the cycle up, half down. */
        if (hz > 0.0)
        {
            s += (tone_ph < 0.5 ? amp : -amp);

            tone_ph += step;

            if (tone_ph >= 1.0) tone_ph -= 1.0;
        }

        for (int v = 0; v < MAX_VOICE; v++)
        {
            if (!voices[v].active) continue;

            Clip *c = &clips[voices[v].clip];

            if (voices[v].at >= c->frames) { voices[v].active = 0; continue; }

            s += c->pcm[voices[v].at++] * voices[v].gain;
        }

        /* Clipped rather than wrapped: a mix that runs hot should get louder
         * and then stop getting louder, not turn into a crackle. */
        if (s >  1.0) s =  1.0;
        if (s < -1.0) s = -1.0;

        out[i] = (float) s;
    }
}

/* Opens the device, once, on the first sound anything asks for.
 *
 * Lazy so that a program which never makes a noise never touches the audio
 * hardware -- which matters for a TUI, and for a test run.
 *
 * The device is asked for float32 mono at 44100 with `allowed_changes` of
 * zero, so SDL either gives exactly that or fails. Letting SDL substitute a
 * format would put a conversion between the mixer and the speaker, and the
 * mixer's whole job is to have already done that work.
 */
static int ensure_open (void)
{
    if (opened) return 1;
    if (is_silent ()) return 0;

    if (SDL_InitSubSystem (SDL_INIT_AUDIO) != 0) return 0;

    SDL_AudioSpec want;
    SDL_AudioSpec got;

    SDL_zero (want);
    SDL_zero (got);

    want.freq     = RATE;
    want.format   = AUDIO_F32SYS;
    want.channels = 1;
    want.samples  = 1024;
    want.callback = mix;

    device = SDL_OpenAudioDevice (NULL, 0, &want, &got, 0);

    if (device == 0)
    {
        SDL_QuitSubSystem (SDL_INIT_AUDIO);

        return 0;
    }

    opened = 1;

    SDL_PauseAudioDevice (device, 0);

    return 1;
}

/* ------------------------------------------------------------- the API -- */

/* Whether anything will actually be heard.
 *
 * False under ALG_SOUND=dummy, and false where no output device could be
 * opened at all. The units report it so a program can say so rather than
 * appearing to work.
 */
int64_t alg_sound_audible (void)
{
    if (is_silent ()) return 0;

    return ensure_open () ? 1 : 0;
}

/* Starts a square wave at Hz, or stops it when Hz is zero or less. */
void alg_sound_tone (int64_t hz)
{
    if (hz <= 0) { tone_hz = 0.0; return; }
    if (!ensure_open ()) return;

    tone_hz = (double) hz;
}

/* Stops the tone. The samples carry on, a sound effect not being a note. */
void alg_sound_off (void)
{
    tone_hz = 0.0;
}

/* The tone's loudness, 0 to 255. */
void alg_sound_volume (int64_t level)
{
    if (level < 0)   level = 0;
    if (level > 255) level = 255;

    tone_amp = (double) level / 255.0 * 0.7;
}

/* Decodes a WAV file into memory and answers a handle, or -1.
 *
 * WAV only, SDL2 carrying one decoder. Decoded once, up front, into the
 * mixer's own format -- float32 mono at 44100 -- so playing it later costs
 * nothing but the mixing, which is what a sound effect needs.
 *
 * Asks nothing of the device, so this works under ALG_SOUND=dummy and before
 * anything has made a noise. That is deliberate: it keeps a decode testable
 * on a machine with no audio hardware.
 */
int64_t alg_sound_load (const char *path)
{
    if (path == NULL) return -1;

    int slot = -1;

    for (int i = 0; i < MAX_CLIPS; i++)
        if (clips[i].pcm == NULL) { slot = i; break; }

    if (slot < 0) return -1;

    SDL_AudioSpec spec;
    Uint8        *raw = NULL;
    Uint32        len = 0;

    if (SDL_LoadWAV (path, &spec, &raw, &len) == NULL) return -1;

    /* Into the mixer's one format, whatever the file's was. */
    SDL_AudioCVT cvt;

    int needed = SDL_BuildAudioCVT (&cvt, spec.format, spec.channels, spec.freq,
                                    AUDIO_F32SYS, 1, RATE);

    if (needed < 0) { SDL_FreeWAV (raw); return -1; }

    float  *pcm   = NULL;
    int64_t frames = 0;

    if (needed == 0)
    {
        /* Already the mixer's format, so the bytes are the samples. */
        frames = (int64_t) (len / sizeof (float));

        if (frames <= 0 || frames > MAX_FRAMES) { SDL_FreeWAV (raw); return -1; }

        pcm = (float *) calloc ((size_t) frames, sizeof (float));

        if (pcm == NULL) { SDL_FreeWAV (raw); return -1; }

        memcpy (pcm, raw, (size_t) frames * sizeof (float));

        SDL_FreeWAV (raw);
    }
    else
    {
        cvt.len = (int) len;
        cvt.buf = (Uint8 *) SDL_malloc ((size_t) cvt.len * (size_t) cvt.len_mult);

        if (cvt.buf == NULL) { SDL_FreeWAV (raw); return -1; }

        memcpy (cvt.buf, raw, len);

        SDL_FreeWAV (raw);

        if (SDL_ConvertAudio (&cvt) != 0) { SDL_free (cvt.buf); return -1; }

        frames = (int64_t) ((size_t) cvt.len_cvt / sizeof (float));

        if (frames <= 0 || frames > MAX_FRAMES) { SDL_free (cvt.buf); return -1; }

        /* Copied out of SDL's allocation into ours, so that one free () in
         * alg_sound_free serves every clip however it arrived. */
        pcm = (float *) calloc ((size_t) frames, sizeof (float));

        if (pcm == NULL) { SDL_free (cvt.buf); return -1; }

        memcpy (pcm, cvt.buf, (size_t) frames * sizeof (float));

        SDL_free (cvt.buf);
    }

    clips[slot].pcm    = pcm;
    clips[slot].frames = frames;

    return slot;
}

/* How many frames a loaded sound holds, or -1 for a handle that is not one. */
int64_t alg_sound_frames (int64_t clip)
{
    if (clip < 0 || clip >= MAX_CLIPS || clips[clip].pcm == NULL) return -1;

    return clips[clip].frames;
}

/* The mixer's sample rate, so the unit can turn frames into milliseconds. */
int64_t alg_sound_rate (void)
{
    return (int64_t) RATE;
}

/* Starts a loaded sound playing, at a volume of 0 to 255.
 *
 * Answers 1 if a voice was found for it and 0 if every voice was busy -- a
 * sound effect that arrives when sixteen are already sounding is dropped
 * rather than cutting one short.
 */
int64_t alg_sound_play (int64_t clip, int64_t volume)
{
    if (clip < 0 || clip >= MAX_CLIPS || clips[clip].pcm == NULL) return 0;
    if (!ensure_open ()) return 0;

    if (volume < 0)   volume = 0;
    if (volume > 255) volume = 255;

    int64_t started = 0;

    hold ();

    for (int v = 0; v < MAX_VOICE; v++)
        if (!voices[v].active)
        {
            voices[v].clip   = clip;
            voices[v].at     = 0;
            voices[v].gain   = (double) volume / 255.0;
            voices[v].active = 1;

            started = 1;

            break;
        }

    release ();

    return started;
}

/* Whether any voice is still playing that sound. */
int64_t alg_sound_playing (int64_t clip)
{
    for (int v = 0; v < MAX_VOICE; v++)
        if (voices[v].active && voices[v].clip == clip) return 1;

    return 0;
}

/* Silences every sound effect, leaving the tone alone. */
void alg_sound_hush (void)
{
    hold ();

    for (int v = 0; v < MAX_VOICE; v++) voices[v].active = 0;

    release ();
}

/* Forgets a loaded sound and gives its memory back.
 *
 * Any voice playing it is stopped first, under the lock, so the mixer cannot
 * be part way through reading the samples as they go.
 */
void alg_sound_free (int64_t clip)
{
    if (clip < 0 || clip >= MAX_CLIPS || clips[clip].pcm == NULL) return;

    hold ();

    for (int v = 0; v < MAX_VOICE; v++)
        if (voices[v].clip == clip) voices[v].active = 0;

    free (clips[clip].pcm);

    clips[clip].pcm    = NULL;
    clips[clip].frames = 0;

    release ();
}

/* Stops everything and closes the device.
 *
 * Called by CloseGraph and CloseCrt, and safe to call when nothing is open.
 */
void alg_sound_close (void)
{
    alg_sound_off ();
    alg_sound_hush ();

    hold ();

    for (int i = 0; i < MAX_CLIPS; i++)
        if (clips[i].pcm != NULL)
        {
            free (clips[i].pcm);

            clips[i].pcm    = NULL;
            clips[i].frames = 0;
        }

    release ();

    if (!opened) return;

    SDL_CloseAudioDevice (device);
    SDL_QuitSubSystem (SDL_INIT_AUDIO);

    device  = 0;
    opened  = 0;
    tone_ph = 0.0;
}
