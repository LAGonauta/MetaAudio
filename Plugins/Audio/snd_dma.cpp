#include <metahook.h>
#include "FileSystem.h"
#include "util.h"
#include "exportfuncs.h"
#include "snd_local.h"
#include "zone.h"

//sfx struct
sfx_t known_sfx[MAX_SFX];
int num_sfx;

//channels
aud_channel_t channels[MAX_CHANNELS];
int total_channels;

//engine cvars
cvar_t *nosound = NULL;
cvar_t *volume = NULL;
cvar_t *loadas8bit = NULL;
cvar_t *sxroom_off = NULL;
cvar_t *sxroomwater_type = NULL;
cvar_t *sxroom_type = NULL;
cvar_t *snd_show = NULL;

//active control
cvar_t *al_enable = NULL;
qboolean openal_started = false;
qboolean openal_enabled = false;
qboolean opanal_mute = false;

//other cvars

//OpenAL device
char al_device_name[64] = "";
int al_device_majorversion = 0;
int al_device_minorversion = 0;

// translates from AL coordinate system to quake
// HL seems to use centimeters, convert to meters.
#define AL_UnpackVector(v) -v[1] * 0.01, v[2] * 0.01, -v[0] * 0.01
#define AL_CopyVector(a, b) ((b)[0] = -(a)[1], (b)[1] = (a)[2], (b)[2] = -(a)[0])

void S_FreeCache(sfx_t *sfx)
{
    aud_sfxcache_t *sc = (aud_sfxcache_t *)Cache_Check(&sfx->cache);
    if (!sc)
        return;

    //2015-12-12 fixed a bug that a buffer in use is freed
    if (SND_IsPlaying(sfx))
        return;

    if (openal_enabled)
    {
        qalGetError();

        if (sc->albuffer)
        {
            qalDeleteBuffers(1, &sc->albuffer);
        }

        int err;
        if ((err = qalGetError()) != AL_NO_ERROR)
        {
            gEngfuncs.Con_DPrintf("S_CheckWavEnd: got an error %d.\n", err);
        }

        if (sc->file)
        {
            g_pFileSystem->Close(sc->file);
        }
    }

    Cache_Free(&sfx->cache);
}

void S_FlushCaches(void)
{
    for (int i = 0; i < num_sfx; ++i)
    {
        S_FreeCache(&known_sfx[i]);
    }
}

sfx_t *S_FindName(char *name, int *pfInCache)
{
    int i = 0;
    sfx_t *sfx = NULL;

    if (!name)
        Sys_ErrorEx("S_FindName: NULL\n");

    if (strlen(name) >= MAX_QPATH)
        Sys_ErrorEx("Sound name too long: %s", name);

    for (i = 0; i < num_sfx; i++)
    {
        if (!stricmp(known_sfx[i].name, name))
        {
            if (pfInCache)
            {
                *pfInCache = Cache_Check(&known_sfx[i].cache) ? 1 : 0;
            }

            if (known_sfx[i].servercount > 0)
                known_sfx[i].servercount = *gAudEngine.cl_servercount;

            return &known_sfx[i];
        }

        if (!sfx)
        {
            if (known_sfx[i].servercount > 0)
            {
                if (known_sfx[i].servercount != *gAudEngine.cl_servercount)
                    sfx = &known_sfx[i];
            }
        }
    }

    if (!sfx)
    {
        if (num_sfx == MAX_SFX)
            Sys_ErrorEx("S_FindName: out of sfx_t");

        sfx = &known_sfx[i];
        num_sfx++;
    }
    else
    {
        //free OpenAL buffer and cache
        S_FreeCache(sfx);
    }

    strncpy(sfx->name, name, sizeof(sfx->name) - 1);
    sfx->name[sizeof(sfx->name) - 1] = 0;

    if (pfInCache)
        *pfInCache = 0;

    sfx->servercount = *gAudEngine.cl_servercount;
    return sfx;
}

void S_CheckWavEnd(aud_channel_t *ch, aud_sfxcache_t *sc)
{
    if (ch->voicecache)
    {
        VoiceSE_QueueBuffers(ch);
        return;
    }

    if (!sc)
        return;

    //Queue buffers for stream sound
    if (ch->alstreambuffers[0])
    {
        ALint iBuffersProcessed;
        ALuint uiBuffer;

        qalGetSourcei(ch->source, AL_BUFFERS_PROCESSED, &iBuffersProcessed);

        while (iBuffersProcessed)
        {
            qalSourceUnqueueBuffers(ch->source, 1, &uiBuffer);

            if (S_StreamLoadNextChunk(ch, sc, uiBuffer))
            {
                qalSourceQueueBuffers(ch->source, 1, &uiBuffer);
            }

            iBuffersProcessed--;
        }
    }

    qboolean fWaveEnd = false;

    ALint iSourceState;
    qalGetSourcei(ch->source, AL_SOURCE_STATE, &iSourceState);

    if (iSourceState == AL_STOPPED)
    {
        fWaveEnd = true;
    }
    else if (!ch->alstreambuffers[0])
    {
        ALint iSamplesPlayed;
        qalGetSourcei(ch->source, AL_SAMPLE_OFFSET, &iSamplesPlayed);

        if (sc->loopstart != -1)
        {
            int iSampleOneFrame = -sc->speed * (*gAudEngine.cl_time - *gAudEngine.cl_oldtime);
            //Wave End
            if (iSamplesPlayed - ch->end >= iSampleOneFrame)
            {
                fWaveEnd = true;
            }
        }
        else
        {
            if (iSamplesPlayed >= ch->end)
            {
                fWaveEnd = true;
            }
        }
    }

    if (!fWaveEnd)
        return;

    if (sc->loopstart != -1)
    {
        if (sc->loopstart < ch->end)
        {
            qalSourcei(ch->source, AL_SAMPLE_OFFSET, sc->loopstart);
            qalSourcePlay(ch->source);
        }
        return;
    }

    if (ch->alstreambuffers[0])
    {
        ALint iQueuedBuffers;
        qalGetSourcei(ch->source, AL_BUFFERS_QUEUED, &iQueuedBuffers);

        // If there are Buffers in the Source Queue then the Source was starved of audio
        // data, so needs to be restarted (because there is more audio data to play)
        if (iQueuedBuffers)
        {
            qalSourcePlay(ch->source);
            return;
        }
    }

    if (ch->isentence >= 0)
    {
        sfx_t *sfx;

        //2015-12-12 fixed, stop the channel before free buffer
        qalSourceStop(ch->source);

        if (rgrgvoxword[ch->isentence][ch->iword].sfx && !rgrgvoxword[ch->isentence][ch->iword].fKeepCached)
            S_FreeCache(rgrgvoxword[ch->isentence][ch->iword].sfx);

        ch->sfx = sfx = rgrgvoxword[ch->isentence][ch->iword + 1].sfx;

        if (sfx)
        {
            sc = S_LoadSound(sfx, ch);

            if (sc)
            {
                ch->start = 0;
                ch->end = sc->length;
                ch->iword++;

                VOX_TrimStartEndTimes(ch, sc);
                qalSourcei(ch->source, AL_SAMPLE_OFFSET, ch->start);
                qalSourcePlay(ch->source);
                return;
            }
        }

        S_FreeChannel(ch);
    }
}

void SND_Spatialize(aud_channel_t *ch, qboolean init)
{
    if (!ch->sfx)
        return;

    //invalid source
    if (!qalIsSource(ch->source))
        return;

    //apply effect
    qboolean underwater = (*gAudEngine.cl_waterlevel > 2) ? true : false;
    int roomtype = 0;

    if (sxroomwater_type && sxroom_type)
    {
        roomtype = underwater ? (int)sxroomwater_type->value : (int)sxroom_type->value;
    }
    SX_ApplyEffect(ch, roomtype, underwater);

    //for later usage
    aud_sfxcache_t *sc = (aud_sfxcache_t *)Cache_Check(&ch->sfx->cache);

    //move mouth
    if (ch->entnum > 0 && (ch->entchannel == CHAN_VOICE || ch->entchannel == CHAN_STREAM))
    {
        if (sc && sc->channels == 1 && sc->width == 1 && sc->datalen > 0)
        {
            SND_MoveMouth(ch, sc);
        }
    }

    //update position
    if (ch->entnum != *gAudEngine.cl_viewentity)
    {
        if (ch->entnum > 0 && ch->entnum < *gAudEngine.cl_num_entities)
        {
            cl_entity_t *pent = gEngfuncs.GetEntityByIndex(ch->entnum);

            if (pent && pent->model && pent->curstate.messagenum == *gAudEngine.cl_parsecount)
            {
                VectorCopy(pent->origin, ch->origin);

                if (pent->model->type == mod_brush)
                {
                    ch->origin[0] = (pent->model->mins[0] + pent->model->maxs[0]) * 0.5;
                    ch->origin[1] = (pent->model->mins[1] + pent->model->maxs[1]) * 0.5;
                    ch->origin[2] = (pent->model->mins[2] + pent->model->maxs[2]) * 0.5;
                }
            }
        }
        qalSource3f(ch->source, AL_POSITION, AL_UnpackVector(ch->origin));
        qalSourcei(ch->source, AL_SOURCE_RELATIVE, AL_FALSE);
    }
    else
    {
        qalSource3f(ch->source, AL_POSITION, 0, 0, 0);
        qalSourcei(ch->source, AL_SOURCE_RELATIVE, AL_TRUE);
    }

    float fvol = 1.0f;
    float fpitch = 1.0f;

    VOX_SetChanVolPitch(ch, &fvol, &fpitch);

    if (sc && sc->length != 0x40000000)
    {
        fvol *= (*gAudEngine.g_SND_VoiceOverdrive);
    }

    qalSourcef(ch->source, AL_GAIN, ch->volume * fvol);
    qalSourcef(ch->source, AL_PITCH, ch->pitch * fpitch);

    if (!init)
    {
        S_CheckWavEnd(ch, sc);
    }
}

void S_Update(float *origin, float *forward, float *right, float *up)
{
    int i, total;
    vec_t orientation[6];
    aud_channel_t *ch;

    if (openal_started)
    {
        if (openal_enabled && !al_enable->value)
        {
            S_StopAllSounds(true);
            S_FlushCaches();
            openal_enabled = false;
        }
        else if (!openal_enabled && al_enable->value)
        {
            S_StopAllSounds(true);
            S_FlushCaches();
            openal_enabled = true;
        }
    }

    if (!openal_enabled)
    {
        return gAudEngine.S_Update(origin, forward, right, up);
    }

    AL_CopyVector(forward, orientation);
    AL_CopyVector(up, orientation + 3);

    if (opanal_mute)
    {
        qalListenerf(AL_GAIN, 0);
    }
    else
    {
        if (volume)
            qalListenerf(AL_GAIN, max(min(volume->value, 1), 0));
        else
            qalListenerf(AL_GAIN, 1);
    }

    qalListener3f(AL_POSITION, AL_UnpackVector(origin));
    qalListenerfv(AL_ORIENTATION, orientation);

    for (i = NUM_AMBIENTS, ch = channels + NUM_AMBIENTS; i < total_channels; i++, ch++)
    {
        SND_Spatialize(ch, false);
    }

    if (snd_show && snd_show->value)
    {
        total = 0;
        ch = channels;
        for (i = 0; i < total_channels; i++, ch++)
        {
            if (ch->sfx && ch->volume > 0)
            {
                gEngfuncs.Con_Printf("%3i %s\n", (int)(ch->volume * 255.0f), ch->sfx->name);
                total++;
            }
        }

        gEngfuncs.Con_Printf("----(%i)----\n", total);
    }
}

void S_FreeChannel(aud_channel_t *ch)
{
    if (qalIsSource(ch->source))
    {
        // Stop the Source and reset the Buffer
        qalSourceStop(ch->source);
        qalSourcei(ch->source, AL_BUFFER, 0);
    }

    if (ch->alstreambuffers[0])
    {
        //free stream buffer for me
        qalDeleteBuffers(4, ch->alstreambuffers);
        ch->alstreambuffers[0] = 0;
        ch->alstreambuffers[1] = 0;
        ch->alstreambuffers[2] = 0;
        ch->alstreambuffers[3] = 0;
    }

    if (ch->entchannel >= CHAN_NETWORKVOICE_BASE && ch->entchannel <= CHAN_NETWORKVOICE_END)
        VoiceSE_NotifyFreeChannel(ch);

    if (ch->isentence >= 0)
        rgrgvoxword[ch->isentence][0].sfx = NULL;

    ch->isentence = -1;
    ch->sfx = NULL;

    SND_CloseMouth(ch);
}

int S_AlterChannel(int entnum, int entchannel, sfx_t *sfx, float fvol, float pitch, int flags)
{
    int ch_idx;
    aud_channel_t *ch;

    if (sfx->name[0] == '!')
    {
        // This is a sentence name.
        // For sentences: assume that the entity is only playing one sentence
        // at a time, so we can just shut off
        // any channel that has ch->isentence >= 0 and matches the
        // soundsource.

        for (ch_idx = 0; ch_idx < total_channels; ch_idx++)
        {
            ch = &channels[ch_idx];
            if (ch->entnum == entnum
                && ch->entchannel == entchannel
                && ch->sfx != NULL
                && ch->isentence >= 0)
            {

                if (flags & SND_CHANGE_PITCH)
                    ch->pitch = pitch;

                if (flags & SND_CHANGE_VOL)
                    ch->volume = fvol;

                if (flags & SND_STOP)
                {
                    S_FreeChannel(ch);
                }

                return TRUE;
            }
        }
        // channel not found
        return FALSE;
    }

    for (ch_idx = 0; ch_idx < total_channels; ch_idx++)
    {
        ch = &channels[ch_idx];
        if (ch->entnum == entnum
            && ch->entchannel == entchannel
            && ch->sfx == sfx)
        {
            if (flags & SND_CHANGE_PITCH)
                ch->pitch = pitch;

            if (flags & SND_CHANGE_VOL)
                ch->volume = fvol;

            if (flags & SND_STOP)
            {
                S_FreeChannel(ch);
            }

            return TRUE;
        }
    }

    return FALSE;
}

qboolean SND_IsPlaying(sfx_t *sfx)
{
    int ch_idx;

    for (ch_idx = 0; ch_idx < MAX_CHANNELS; ch_idx++)
    {
        if (channels[ch_idx].sfx == sfx)
            return true;
    }

    return false;
}

aud_channel_t *SND_PickDynamicChannel(int entnum, int entchannel, sfx_t *sfx)
{
    int ch_idx;
    int first_to_die;

    if (entchannel == CHAN_STREAM && SND_IsPlaying(sfx))
        return NULL;

    first_to_die = -1;

    float life_left = 99999;
    float life;
    int state, played;

    aud_channel_t *ch;
    aud_sfxcache_t *sc;

    // Check all channels and check if it is available for use
    for (ch_idx = NUM_AMBIENTS; ch_idx < NUM_AMBIENTS + MAX_DYNAMIC_CHANNELS; ch_idx++)
    {
        ch = &channels[ch_idx];
        if (ch->entchannel == CHAN_STREAM && ch->alstreambuffers[0])
        {
            if (entchannel == CHAN_VOICE)
                return NULL;

            continue;
        }

        // Appointed channel
        if (entchannel != 0 && ch->entnum == entnum && (ch->entchannel == entchannel || entchannel == -1))
        {
            first_to_die = ch_idx;
            break;
        }

        if (ch->entnum == *gAudEngine.cl_viewentity && entnum != *gAudEngine.cl_viewentity && ch->sfx)
            continue;

        if (!qalIsSource(ch->source))
        {
            first_to_die = ch_idx;
            break;
        }

        if (ch->sfx == NULL)
        {
            first_to_die = ch_idx;
            break;
        }

        qalGetSourcei(ch->source, AL_SOURCE_STATE, &state);
        if (state == AL_STOPPED)
        {
            first_to_die = ch_idx;
            break;
        }

        // Sometimes the cache_check crashes, lets comment it out for now
        //sc = (aud_sfxcache_t *)Cache_Check(&sfx->cache);
        if (sfx->cache.data != nullptr)
        {
            sc = (aud_sfxcache_t *)(sfx->cache.data);
        }

        if (sc == nullptr)
        {
            first_to_die = ch_idx;
            break;
        }

        qalGetSourcei(ch->source, AL_SAMPLE_OFFSET, &played);
        life = (float)(ch->end - played) / (float)sc->speed;

        if (life < life_left)
        {
            life_left = life;
            first_to_die = ch_idx;
        }
    }

    if (first_to_die == -1)
        return NULL;

    if (channels[first_to_die].sfx)
    {
        S_FreeChannel(&(channels[first_to_die]));
    }

    return &channels[first_to_die];
}

void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch)
{
    if (!openal_enabled)
    {
        return gAudEngine.S_StartDynamicSound(entnum, entchannel, sfx, origin, fvol, attenuation, flags, pitch);
    }

    aud_channel_t *ch;
    aud_sfxcache_t *sc;
    qboolean fsentence;
    float fpitch;

    if (!sfx)
        return;

    if (nosound && nosound->value)
        return;

    if (sfx->name[0] == '*')
        entchannel = CHAN_STREAM;

    if (entchannel == CHAN_STREAM && pitch != 100)
    {
        gEngfuncs.Con_DPrintf("Warning: pitch shift ignored on stream sound %s\n", sfx->name);
        pitch = 100;
    }

    if (fvol > 1.0f)
    {
        gEngfuncs.Con_DPrintf("S_StartDynamicSound: %s fvolume > 1.0", sfx->name);
    }

    fpitch = pitch / 100.0f;

    if (flags & (SND_STOP | SND_CHANGE_VOL | SND_CHANGE_PITCH))
    {
        if (S_AlterChannel(entnum, entchannel, sfx, fvol, fpitch, flags))
            return;

        if (flags & SND_STOP)
            return;
    }

    if (pitch == 0)
    {
        gEngfuncs.Con_DPrintf("Warning: S_StartDynamicSound Ignored, called with pitch 0");
        return;
    }

    ch = SND_PickDynamicChannel(entnum, entchannel, sfx);

    if (!ch)
        return;

    qalGetError();

    VectorCopy(origin, ch->origin);
    ch->attenuation = attenuation;
    ch->volume = fvol;
    ch->entnum = entnum;
    ch->entchannel = entchannel;
    ch->pitch = fpitch;
    ch->isentence = -1;

    if (sfx->name[0] == '!' || sfx->name[0] == '#')
    {
        char name[MAX_QPATH];
        strncpy(name, sfx->name + 1, sizeof(name) - 1);
        name[sizeof(name) - 1] = 0;
        sc = VOX_LoadSound(ch, name);
        fsentence = true;
    }
    else
    {
        sc = S_LoadSound(sfx, ch);
        ch->sfx = sfx;
        fsentence = false;
    }

    if (!sc)
    {
        ch->sfx = NULL;
        return;
    }

    if (!qalIsSource(ch->source))
    {
        qalGenSources(1, &ch->source);
    }

    ch->start = 0;
    ch->end = sc->length;

    if (!fsentence && ch->pitch != 1)
        VOX_MakeSingleWordSentence(ch, pitch);

    VOX_TrimStartEndTimes(ch, sc);

    SND_InitMouth(entnum, entchannel);

    qalSourcef(ch->source, AL_ROLLOFF_FACTOR, ch->attenuation);
    qalSourcei(ch->source, AL_SAMPLE_OFFSET, ch->start);

    if (ch->alstreambuffers[0])
        qalSourceQueueBuffers(ch->source, 4, ch->alstreambuffers);
    else
        qalSourcei(ch->source, AL_BUFFER, sc->albuffer);

    SND_Spatialize(ch, true);

    qalSourcePlay(ch->source);

    int err;
    if ((err = qalGetError()) != AL_NO_ERROR)
    {
        S_FreeChannel(ch);
        gEngfuncs.Con_DPrintf("S_StartStaticSound: got an error 0x%X on %s.\n", err, ch->sfx ? ch->sfx->name : "");
    }
}

aud_channel_t *SND_PickStaticChannel(int entnum, int entchannel, sfx_t *sfx)
{
    int i;
    aud_channel_t *ch = NULL;

    for (i = MAX_DYNAMIC_CHANNELS; i < total_channels; i++)
    {
        if (channels[i].sfx == NULL)
            break;

        // This should allow channels to be reused, but won't work on some
        // bugged Creative drivers.
        if (qalIsSource(channels[i].source))
        {
            ALint state;
            qalGetSourcei(channels[i].source, AL_SOURCE_STATE, &state);
            if (state == AL_STOPPED)
            {
                qalSourcei(channels[i].source, AL_BUFFER, 0);
                break;
            }
        }
    }

    if (i < total_channels)
    {
        ch = &channels[i];
    }
    else
    {
        // no empty slots, alloc a new static sound channel
        if (total_channels == MAX_CHANNELS)
        {
            gEngfuncs.Con_DPrintf("total_channels == MAX_CHANNELS\n");
            return NULL;
        }

        // get a channel for the static sound
        ch = &channels[total_channels];
        total_channels++;
    }

    return ch;
}

void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch)
{
    aud_channel_t *ch;
    aud_sfxcache_t *sc;
    qboolean fsentence;
    float fpitch;

    if (!openal_enabled)
    {
        return gAudEngine.S_StartStaticSound(entnum, entchannel, sfx, origin, fvol, attenuation, flags, pitch);
    }

    if (!sfx)
        return;

    if (nosound && nosound->value)
        return;

    if (sfx->name[0] == '*')
        entchannel = CHAN_STREAM;

    if (entchannel == CHAN_STREAM && pitch != 100)
    {
        gEngfuncs.Con_DPrintf("Warning: pitch shift ignored on stream sound %s\n", sfx->name);
        pitch = 100;
    }

    if (fvol > 1.0f)
    {
        gEngfuncs.Con_DPrintf("S_StartStaticSound: %s fvolume > 1.0", sfx->name);
    }

    fpitch = pitch / 100.0f;

    if (flags & (SND_STOP | SND_CHANGE_VOL | SND_CHANGE_PITCH))
    {
        if (S_AlterChannel(entnum, entchannel, sfx, fvol, fpitch, flags))
            return;

        if (flags & SND_STOP)
            return;
    }

    if (pitch == 0)
    {
        gEngfuncs.Con_DPrintf("Warning: S_StartStaticSound Ignored, called with pitch 0");
        return;
    }

    ch = SND_PickStaticChannel(entnum, entchannel, sfx);

    if (!ch)
        return;

    qalGetError();

    VectorCopy(origin, ch->origin);
    ch->attenuation = attenuation;
    ch->volume = fvol;
    ch->entnum = entnum;
    ch->entchannel = entchannel;
    ch->pitch = fpitch;
    ch->isentence = -1;

    if (sfx->name[0] == '!' || sfx->name[0] == '#')
    {
        char name[MAX_QPATH];
        strncpy(name, sfx->name + 1, sizeof(name) - 1);
        name[sizeof(name) - 1] = 0;
        sc = VOX_LoadSound(ch, name);
        fsentence = true;
    }
    else
    {
        sc = S_LoadSound(sfx, ch);
        ch->sfx = sfx;
        fsentence = false;
    }

    if (!sc)
    {
        ch->sfx = NULL;
        return;
    }

    if (!qalIsSource(ch->source))
    {
        qalGenSources(1, &ch->source);
    }

    ch->start = 0;
    ch->end = sc->length;

    if (!fsentence && ch->pitch != 1)
        VOX_MakeSingleWordSentence(ch, pitch);

    VOX_TrimStartEndTimes(ch, sc);

    qalSourcef(ch->source, AL_ROLLOFF_FACTOR, ch->attenuation);
    qalSourcei(ch->source, AL_SAMPLE_OFFSET, ch->start);

    if (ch->alstreambuffers[0])
        qalSourceQueueBuffers(ch->source, 4, ch->alstreambuffers);
    else
        qalSourcei(ch->source, AL_BUFFER, sc->albuffer);

    SND_Spatialize(ch, true);

    qalSourcePlay(ch->source);

    int err;
    if ((err = qalGetError()) != AL_NO_ERROR)
    {
        S_FreeChannel(ch);
        gEngfuncs.Con_DPrintf("S_StartStaticSound: got an error 0x%X on %s.\n", err, ch->sfx ? ch->sfx->name : "");
    }
}

void S_StopSound(int entnum, int entchannel)
{
    if (!openal_enabled)
    {
        return gAudEngine.S_StopSound(entnum, entchannel);
    }

    for (int i = NUM_AMBIENTS; i < total_channels; i++)
    {
        if (channels[i].entnum == entnum && channels[i].entchannel == entchannel)
        {
            S_FreeChannel(&channels[i]);
        }
    }
}

void S_StopAllSounds(qboolean clear)
{
    if (!openal_enabled)
    {
        return gAudEngine.S_StopAllSounds(clear);
    }

    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i].sfx)
            S_FreeChannel(&channels[i]);
    }

    memset(channels, 0, MAX_CHANNELS * sizeof(aud_channel_t));
    total_channels = MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS;
}

qboolean OpenAL_Init(void)
{
    ALCcontext *pContext = NULL;
    ALCdevice *pDevice = NULL;

    if (!qal_init)
        return false;

    pDevice = qalcOpenDevice(NULL);
    if (pDevice)
    {
        pContext = qalcCreateContext(pDevice, NULL);
        if (pContext)
        {
            strcpy(al_device_name, qalcGetString(pDevice, ALC_DEVICE_SPECIFIER));
            qalcGetIntegerv(pDevice, ALC_MAJOR_VERSION, 1, &al_device_majorversion);
            qalcGetIntegerv(pDevice, ALC_MINOR_VERSION, 1, &al_device_minorversion);
            qalcMakeContextCurrent(pContext);
            return true;
        }
        else
        {
            qalcCloseDevice(pDevice);
        }
    }

    return false;
}

void S_Startup(void)
{
    gAudEngine.S_Startup();

    //stop mute me first
    opanal_mute = false;

    if (!openal_started)
    {
        if (OpenAL_Init())
        {
            QAL_InitEFXExtension();

            openal_started = true;
            openal_enabled = (al_enable->value) ? true : false;

            SX_Init();
        }
    }
}

void AL_Version_f(void)
{
    if (openal_started)
        gEngfuncs.Con_Printf("%s\n OpenAL Device: %s\n OpenAL Version: %d.%d\n EAX Extension: %s\n", META_AUDIO_VERSION, al_device_name, al_device_majorversion, al_device_minorversion, qal_efxinit ? "ok" : "no");
    else
        gEngfuncs.Con_Printf("%s\n Failed to initalize OpenAL device.\n", META_AUDIO_VERSION, al_device_name, al_device_majorversion, al_device_minorversion);
}

void S_Init(void)
{
    al_enable = gEngfuncs.pfnRegisterVariable("al_enable", "1", 0);
    gEngfuncs.pfnAddCommand("al_version", AL_Version_f);

    gAudEngine.S_Init();

    memset(known_sfx, 0, sizeof(sfx_t) * MAX_SFX);
    num_sfx = 0;

    VOX_Init();

    if (!gEngfuncs.CheckParm("-nosound", NULL))
    {
        nosound = gEngfuncs.pfnGetCvarPointer("nosound");
        volume = gEngfuncs.pfnGetCvarPointer("volume");
        loadas8bit = gEngfuncs.pfnGetCvarPointer("loadas8bit");
        sxroom_off = gEngfuncs.pfnGetCvarPointer("room_off");
        sxroomwater_type = gEngfuncs.pfnGetCvarPointer("waterroom_type");
        sxroom_type = gEngfuncs.pfnGetCvarPointer("room_type");
        snd_show = gEngfuncs.pfnGetCvarPointer("snd_show");

        S_StopAllSounds(true);
    }
}

void OpenAL_Shutdown(void)
{
    ALCcontext *pContext;
    ALCdevice *pDevice;

    pContext = qalcGetCurrentContext();
    pDevice = qalcGetContextsDevice(pContext);

    qalcMakeContextCurrent(NULL);
    qalcDestroyContext(pContext);
    qalcCloseDevice(pDevice);
}

void S_ShutdownAL(void)
{
    if (openal_started)
    {
        S_StopAllSounds(true);
        SX_Shutdown();
        OpenAL_Shutdown();
        openal_started = false;
    }
}

void S_Shutdown(void)
{
    //shall we mute OpenAL sound when S_Shutdown?
    opanal_mute = true;

    gAudEngine.S_Shutdown();
}