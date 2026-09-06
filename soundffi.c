/* soundffi.c -- the noise the language cannot make.
 *
 * CoreAudio rather than SDL, deliberately.  `crt` is a terminal unit and owes
 * nothing to a window system; making it depend on SDL merely to beep would be
 * the wrong trade.  CoreAudio is a system framework -- present on every Mac,
 * nothing to install -- so all three units can share this one backend and
 * `crt` keeps its promise.
 *
 * That does bind this file to macOS, where SDL would have been portable. It is
 * a smaller loss than it looks: `graph` already names
 * /opt/homebrew/lib/libSDL2.dylib literally, so a Linux port needs work here
 * either way.  It also buys something -- ExtAudioFile decodes WAV, AIFF, CAF,
 * MP3 and M4A, where SDL2 alone reads WAV and nothing else.
 *
 * The split is the house one.  This file makes a square wave, mixes some
 * decoded samples into it, and stops.  Which note, how long, what a dotted
 * quaver is worth at 120 beats a minute -- all of that is the unit's, in
 * Algol-24, where it can be read and tested.
 *
 * Every function is total: called before the device is open, after it is
 * closed, or with a handle that was never valid, each answers something
 * defined rather than failing.
 */

#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define RATE      44100.0
#define MAX_CLIPS 64
#define MAX_VOICE 16

/* ----------------------------------------------------------- the state -- */

typedef struct { float *pcm; int64_t frames; } Clip;
typedef struct { int64_t clip; int64_t at; double gain; int active; } Voice;

static AudioUnit unit;
static int       opened = 0;
static int       silent = -1;      /* -1 until the environment is consulted */

/* The tone.  Written from the calling thread and read from the audio thread;
 * both are scalars a store makes visible whole, and a note starting a
 * fraction of a buffer early or late is not something an ear can hear. */
static volatile double tone_hz  = 0.0;
static volatile double tone_amp = 0.18;
static double          tone_ph  = 0.0;

static Clip  clips[MAX_CLIPS];
static Voice voices[MAX_VOICE];

/* Whether to open the device at all.
 *
 * ALG_SOUND=dummy computes everything and plays nothing, which is what lets
 * the library's own tests run without filling the room -- the same bargain
 * SDL_VIDEODRIVER=dummy strikes for `graph`.
 */
static int is_silent (void)
{
    if (silent < 0)
    {
        const char *s = getenv ("ALG_SOUND");

        silent = (s != NULL && strcmp (s, "dummy") == 0) ? 1 : 0;
    }

    return silent;
}

/* ---------------------------------------------------------- the render -- */

static OSStatus render (void *ref, AudioUnitRenderActionFlags *flags,
                        const AudioTimeStamp *stamp, UInt32 bus,
                        UInt32 frames, AudioBufferList *io)
{
    (void) ref; (void) flags; (void) stamp; (void) bus;

    float *out = (float *) io->mBuffers[0].mData;

    double hz   = tone_hz;
    double amp  = tone_amp;
    double step = hz / RATE;

    for (UInt32 i = 0; i < frames; i++)
    {
        double s = 0.0;

        /* A square wave, which is what a PC speaker made and what Sound has
         * always meant.  Half the cycle up, half down. */
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

    return noErr;
}

/* Opens the device, once, on the first sound anything asks for.
 *
 * Lazy so that a program which never makes a noise never touches the audio
 * hardware -- which matters for a TUI, and for a test run.
 */
static int ensure_open (void)
{
    if (opened) return 1;
    if (is_silent ()) return 0;

    AudioComponentDescription want = {0};

    want.componentType         = kAudioUnitType_Output;
    want.componentSubType      = kAudioUnitSubType_DefaultOutput;
    want.componentManufacturer = kAudioUnitManufacturer_Apple;

    AudioComponent comp = AudioComponentFindNext (NULL, &want);

    if (comp == NULL) return 0;
    if (AudioComponentInstanceNew (comp, &unit) != noErr) return 0;

    AudioStreamBasicDescription fmt = {0};

    fmt.mSampleRate       = RATE;
    fmt.mFormatID         = kAudioFormatLinearPCM;
    fmt.mFormatFlags      = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    fmt.mChannelsPerFrame = 1;
    fmt.mBitsPerChannel   = 32;
    fmt.mBytesPerFrame    = 4;
    fmt.mFramesPerPacket  = 1;
    fmt.mBytesPerPacket   = 4;

    AudioUnitSetProperty (unit, kAudioUnitProperty_StreamFormat,
                          kAudioUnitScope_Input, 0, &fmt, sizeof fmt);

    AURenderCallbackStruct cb = { render, NULL };

    AudioUnitSetProperty (unit, kAudioUnitProperty_SetRenderCallback,
                          kAudioUnitScope_Input, 0, &cb, sizeof cb);

    if (AudioUnitInitialize (unit) != noErr) return 0;
    if (AudioOutputUnitStart (unit) != noErr) return 0;

    opened = 1;

    return 1;
}

/* ------------------------------------------------------------- the API -- */

/* Whether anything will actually be heard.
 *
 * False under ALG_SOUND=dummy, and false where no output device could be
 * opened at all.  The units report it so a program can say so rather than
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

/* Stops the tone.  The samples carry on, a sound effect not being a note. */
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

/* Decodes an audio file into memory and answers a handle, or -1.
 *
 * Whatever CoreAudio can read: WAV, AIFF, CAF, MP3, M4A.  Decoded once, up
 * front, to float PCM at the mixer's own rate -- so playing it later costs
 * nothing but the mixing, which is what a sound effect needs.
 */
int64_t alg_sound_load (const char *path)
{
    if (path == NULL) return -1;

    int slot = -1;

    for (int i = 0; i < MAX_CLIPS; i++)
        if (clips[i].pcm == NULL) { slot = i; break; }

    if (slot < 0) return -1;

    CFURLRef url = CFURLCreateFromFileSystemRepresentation (
        NULL, (const UInt8 *) path, (CFIndex) strlen (path), false);

    if (url == NULL) return -1;

    ExtAudioFileRef f  = NULL;
    OSStatus        st = ExtAudioFileOpenURL (url, &f);

    CFRelease (url);

    if (st != noErr) return -1;

    AudioStreamBasicDescription want = {0};

    want.mSampleRate       = RATE;
    want.mFormatID         = kAudioFormatLinearPCM;
    want.mFormatFlags      = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    want.mChannelsPerFrame = 1;
    want.mBitsPerChannel   = 32;
    want.mBytesPerFrame    = 4;
    want.mFramesPerPacket  = 1;
    want.mBytesPerPacket   = 4;

    if (ExtAudioFileSetProperty (f, kExtAudioFileProperty_ClientDataFormat,
                                 sizeof want, &want) != noErr)
    {
        ExtAudioFileDispose (f);

        return -1;
    }

    SInt64 total = 0;
    UInt32 sz    = sizeof total;

    ExtAudioFileGetProperty (f, kExtAudioFileProperty_FileLengthFrames,
                             &sz, &total);

    if (total <= 0 || total > 44100 * 600)      /* ten minutes is plenty */
    {
        ExtAudioFileDispose (f);

        return -1;
    }

    float *pcm = (float *) calloc ((size_t) total, sizeof (float));

    if (pcm == NULL) { ExtAudioFileDispose (f); return -1; }

    int64_t got = 0;

    while (got < total)
    {
        UInt32 want_frames = (UInt32) (total - got);

        AudioBufferList list;

        list.mNumberBuffers              = 1;
        list.mBuffers[0].mNumberChannels = 1;
        list.mBuffers[0].mDataByteSize   = (UInt32) (want_frames * 4);
        list.mBuffers[0].mData           = pcm + got;

        UInt32 read = want_frames;

        if (ExtAudioFileRead (f, &read, &list) != noErr) break;
        if (read == 0) break;

        got += read;
    }

    ExtAudioFileDispose (f);

    if (got <= 0) { free (pcm); return -1; }

    clips[slot].pcm    = pcm;
    clips[slot].frames = got;

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

    for (int v = 0; v < MAX_VOICE; v++)
        if (!voices[v].active)
        {
            voices[v].clip   = clip;
            voices[v].at     = 0;
            voices[v].gain   = (double) volume / 255.0;
            voices[v].active = 1;

            return 1;
        }

    return 0;
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
    for (int v = 0; v < MAX_VOICE; v++) voices[v].active = 0;
}

/* Forgets a loaded sound and gives its memory back.
 *
 * Any voice playing it is stopped first, or the mixer would read freed
 * memory a frame later.
 */
void alg_sound_free (int64_t clip)
{
    if (clip < 0 || clip >= MAX_CLIPS || clips[clip].pcm == NULL) return;

    for (int v = 0; v < MAX_VOICE; v++)
        if (voices[v].clip == clip) voices[v].active = 0;

    free (clips[clip].pcm);

    clips[clip].pcm    = NULL;
    clips[clip].frames = 0;
}

/* Stops everything and closes the device.
 *
 * Called by CloseGraph and CloseCrt, and safe to call when nothing is open.
 */
void alg_sound_close (void)
{
    alg_sound_off ();
    alg_sound_hush ();

    for (int i = 0; i < MAX_CLIPS; i++)
        if (clips[i].pcm != NULL)
        {
            free (clips[i].pcm);

            clips[i].pcm    = NULL;
            clips[i].frames = 0;
        }

    if (!opened) return;

    AudioOutputUnitStop (unit);
    AudioUnitUninitialize (unit);
    AudioComponentInstanceDispose (unit);

    opened  = 0;
    tone_ph = 0.0;
}
