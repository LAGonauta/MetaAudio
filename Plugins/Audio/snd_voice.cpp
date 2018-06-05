#include <metahook.h>
#include "exportfuncs.h"
#include "FileSystem.h"
#include "util.h"
#include "snd_local.h"
#include "zone.h"

#define VOICE_BUFFER_SIZE 4096

int (*VoiceSE_GetSoundDataCallback)(sfxcache_s *pCache, char *pCopyBuf, int maxOutDataSize, int samplePos, int sampleCount);

//not done
void VoiceSE_NotifyFreeChannel(aud_channel_t *ch)
{
	ch->voicecache = NULL;
	gAudEngine.VoiceSE_NotifyFreeChannel(ch->entchannel);
}

aud_sfxcache_t *VoiceSE_FillSFXCache(sfxcache_t *oldsc)
{
	static aud_sfxcache_t sc;

	sc.length = oldsc->length;
	sc.channels = oldsc->stereo ? 2 : 1;
	sc.speed = oldsc->speed;
	sc.width = oldsc->width;
	sc.albuffer = 0;
	sc.bitrate = sc.speed * sc.width;
	sc.blockalign = sc.width * sc.channels;

	if(sc.channels == 1)
		sc.alformat = (sc.width == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
	else
		sc.alformat = (sc.width == 1) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;

	return &sc;
}

qboolean VoiceSE_GetNextChunk(aud_channel_t *ch, ALuint alBuffer)
{
	//invalid voice?
	if(!ch->voicecache)
		return false;

	aud_sfxcache_t *sc = VoiceSE_FillSFXCache(ch->voicecache);

	size_t ulCopyBufferSize = sc->bitrate >> 2;
	char *pCopyBuffer = (char *)Hunk_TempAlloc( ulCopyBufferSize + 1 );

	size_t ulExpectSamples = ulCopyBufferSize / (sc->width * sc->channels);
	size_t ulRecvedSamples = VoiceSE_GetSoundDataCallback(ch->voicecache, pCopyBuffer, ulCopyBufferSize, 0, ulExpectSamples);
	ulRecvedSamples -= (ulRecvedSamples % sc->blockalign);

	qalBufferData(alBuffer, sc->alformat, pCopyBuffer, ulRecvedSamples * sc->width * sc->channels, sc->speed);

	return true;
}

void VoiceSE_QueueBuffers(aud_channel_t *ch)
{
	//Queue buffers for voice
	if (ch->alstreambuffers[0])
	{
		ALint iBuffersProcessed;
		ALuint uiBuffer;

		qalGetSourcei(ch->source, AL_BUFFERS_PROCESSED, &iBuffersProcessed);

		while(iBuffersProcessed)
		{
			qalSourceUnqueueBuffers(ch->source, 1, &uiBuffer);

			if(VoiceSE_GetNextChunk(ch, uiBuffer))
			{
				qalSourceQueueBuffers(ch->source, 1, &uiBuffer);
			}

			iBuffersProcessed--;
		}
	}
}

aud_sfxcache_t *VoiceSE_GetSFXCache(sfx_t *s, aud_channel_t *ch)
{
	sfxcache_t *oldsc = (sfxcache_t *)gAudEngine.S_LoadSound(s, NULL);

	if(!oldsc)
		return NULL;

	VoiceSE_GetSoundDataCallback = (int (*)(sfxcache_s *, char *, int, int, int))oldsc->loopstart;

	if(!ch->alstreambuffers[0])
	{
		qalGenBuffers(4, ch->alstreambuffers);
	}

	//save the sfxcache for later usage
	ch->voicecache = oldsc;

	aud_sfxcache_t *sc = VoiceSE_FillSFXCache(oldsc);

	size_t ulCopyBufferSize = VOICE_BUFFER_SIZE;
	char *pCopyBuffer = (char *)Hunk_TempAlloc( ulCopyBufferSize + 1 );

	size_t ulExpectSamples = ulCopyBufferSize / (sc->width * sc->channels);
	size_t ulRecvedSamples = VoiceSE_GetSoundDataCallback(oldsc, pCopyBuffer, ulCopyBufferSize, 0, ulExpectSamples);

	size_t ulSampleSize = ulRecvedSamples * sc->width * sc->channels;
	ulSampleSize -= (ulSampleSize % sc->blockalign);

	size_t ulBufferSize = ulSampleSize >> 2;
	ulBufferSize -= (ulBufferSize % sc->blockalign);

	size_t ulSubmitSize = 0;

	for(int i = 0; i < 4; ++i)
	{
		if(i == 3)
			qalBufferData(ch->alstreambuffers[i], sc->alformat, pCopyBuffer + ulSubmitSize, ulSampleSize - ulSubmitSize, sc->speed);
		else
			qalBufferData(ch->alstreambuffers[i], sc->alformat, pCopyBuffer + ulSubmitSize, ulBufferSize, sc->speed);
		ulSubmitSize += ulBufferSize;
	}

	return sc;
}