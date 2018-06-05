#include <metahook.h>

#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "OpenAL/efx.h"

void (AL_APIENTRY *qalEnable)(ALenum capability) = NULL;
void (AL_APIENTRY *qalDisable)(ALenum capability) = NULL;
ALboolean (AL_APIENTRY *qalIsEnabled)(ALenum capability) = NULL;
const ALchar* (AL_APIENTRY *qalGetString)(ALenum param) = NULL;
void (AL_APIENTRY *qalGetBooleanv)(ALenum param, ALboolean* data) = NULL;
void (AL_APIENTRY *qalGetIntegerv)(ALenum param, ALint* data) = NULL;
void (AL_APIENTRY *qalGetFloatv)(ALenum param, ALfloat* data) = NULL;
void (AL_APIENTRY *qalGetDoublev)(ALenum param, ALdouble* data) = NULL;
ALboolean (AL_APIENTRY *qalGetBoolean)(ALenum param) = NULL;
ALint (AL_APIENTRY *qalGetInteger)(ALenum param) = NULL;
ALfloat (AL_APIENTRY *qalGetFloat)(ALenum param) = NULL;
ALdouble (AL_APIENTRY *qalGetDouble)(ALenum param) = NULL;
ALenum (AL_APIENTRY *qalGetError)(void) = NULL;
ALboolean (AL_APIENTRY *qalIsExtensionPresent)(const ALchar* extname) = NULL;
void* (AL_APIENTRY *qalGetProcAddress)(const ALchar* fname) = NULL;
ALenum (AL_APIENTRY *qalGetEnumValue)(const ALchar* ename) = NULL;
void (AL_APIENTRY *qalListenerf)(ALenum param, ALfloat value) = NULL;
void (AL_APIENTRY *qalListener3f)(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3) = NULL;
void (AL_APIENTRY *qalListenerfv)(ALenum param, const ALfloat* values) = NULL;
void (AL_APIENTRY *qalListeneri)(ALenum param, ALint value) = NULL;
void (AL_APIENTRY *qalListener3i)(ALenum param, ALint value1, ALint value2, ALint value3) = NULL;
void (AL_APIENTRY *qalListeneriv)(ALenum param, const ALint* values) = NULL;
void (AL_APIENTRY *qalGetListenerf)(ALenum param, ALfloat* value) = NULL;
void (AL_APIENTRY *qalGetListener3f)(ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3) = NULL;
void (AL_APIENTRY *qalGetListenerfv)(ALenum param, ALfloat* values) = NULL;
void (AL_APIENTRY *qalGetListeneri)(ALenum param, ALint* value) = NULL;
void (AL_APIENTRY *qalGetListener3i)(ALenum param, ALint *value1, ALint *value2, ALint *value3) = NULL;
void (AL_APIENTRY *qalGetListeneriv)(ALenum param, ALint* values) = NULL;
void (AL_APIENTRY *qalGenSources)(ALsizei n, ALuint* sources) = NULL;
void (AL_APIENTRY *qalDeleteSources)(ALsizei n, const ALuint* sources) = NULL;
ALboolean (AL_APIENTRY *qalIsSource)(ALuint sid) = NULL;
void (AL_APIENTRY *qalSourcef)(ALuint sid, ALenum param, ALfloat value) = NULL;
void (AL_APIENTRY *qalSource3f)(ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3) = NULL;
void (AL_APIENTRY *qalSourcefv)(ALuint sid, ALenum param, const ALfloat* values) = NULL;
void (AL_APIENTRY *qalSourcei)(ALuint sid, ALenum param, ALint value) = NULL;
void (AL_APIENTRY *qalSource3i)(ALuint sid, ALenum param, ALint value1, ALint value2, ALint value3) = NULL;
void (AL_APIENTRY *qalSourceiv)(ALuint sid, ALenum param, const ALint* values) = NULL;
void (AL_APIENTRY *qalGetSourcef)(ALuint sid, ALenum param, ALfloat* value) = NULL;
void (AL_APIENTRY *qalGetSource3f)(ALuint sid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3) = NULL;
void (AL_APIENTRY *qalGetSourcefv)(ALuint sid, ALenum param, ALfloat* values) = NULL;
void (AL_APIENTRY *qalGetSourcei)(ALuint sid,  ALenum param, ALint* value) = NULL;
void (AL_APIENTRY *qalGetSource3i)(ALuint sid, ALenum param, ALint* value1, ALint* value2, ALint* value3) = NULL;
void (AL_APIENTRY *qalGetSourceiv)(ALuint sid,  ALenum param, ALint* values) = NULL;
void (AL_APIENTRY *qalSourcePlayv)(ALsizei ns, const ALuint *sids) = NULL;
void (AL_APIENTRY *qalSourceStopv)(ALsizei ns, const ALuint *sids) = NULL;
void (AL_APIENTRY *qalSourceRewindv)(ALsizei ns, const ALuint *sids) = NULL;
void (AL_APIENTRY *qalSourcePausev)(ALsizei ns, const ALuint *sids) = NULL;
void (AL_APIENTRY *qalSourcePlay)(ALuint sid) = NULL;
void (AL_APIENTRY *qalSourceStop)(ALuint sid) = NULL;
void (AL_APIENTRY *qalSourceRewind)(ALuint sid) = NULL;
void (AL_APIENTRY *qalSourcePause)(ALuint sid) = NULL;
void (AL_APIENTRY *qalSourceQueueBuffers)(ALuint sid, ALsizei numEntries, const ALuint *bids) = NULL;
void (AL_APIENTRY *qalSourceUnqueueBuffers)(ALuint sid, ALsizei numEntries, ALuint *bids) = NULL;
void (AL_APIENTRY *qalGenBuffers)(ALsizei n, ALuint* buffers) = NULL;
void (AL_APIENTRY *qalDeleteBuffers)(ALsizei n, const ALuint* buffers) = NULL;
ALboolean (AL_APIENTRY *qalIsBuffer)(ALuint bid) = NULL;
void (AL_APIENTRY *qalBufferData)(ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq) = NULL;
void (AL_APIENTRY *qalBufferf)(ALuint bid, ALenum param, ALfloat value) = NULL;
void (AL_APIENTRY *qalBuffer3f)(ALuint bid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3) = NULL;
void (AL_APIENTRY *qalBufferfv)(ALuint bid, ALenum param, const ALfloat* values) = NULL;
void (AL_APIENTRY *qalBufferi)(ALuint bid, ALenum param, ALint value) = NULL;
void (AL_APIENTRY *qalBuffer3i)(ALuint bid, ALenum param, ALint value1, ALint value2, ALint value3) = NULL;
void (AL_APIENTRY *qalBufferiv)(ALuint bid, ALenum param, const ALint* values) = NULL;
void (AL_APIENTRY *qalGetBufferf)(ALuint bid, ALenum param, ALfloat* value) = NULL;
void (AL_APIENTRY *qalGetBuffer3f)(ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3) = NULL;
void (AL_APIENTRY *qalGetBufferfv)(ALuint bid, ALenum param, ALfloat* values) = NULL;
void (AL_APIENTRY *qalGetBufferi)(ALuint bid, ALenum param, ALint* value) = NULL;
void (AL_APIENTRY *qalGetBuffer3i)(ALuint bid, ALenum param, ALint* value1, ALint* value2, ALint* value3) = NULL;
void (AL_APIENTRY *qalGetBufferiv)(ALuint bid, ALenum param, ALint* values) = NULL;
void (AL_APIENTRY *qalDopplerFactor)(ALfloat value) = NULL;
void (AL_APIENTRY *qalDopplerVelocity)(ALfloat value) = NULL;
void (AL_APIENTRY *qalSpeedOfSound)(ALfloat value) = NULL;
void (AL_APIENTRY *qalDistanceModel)(ALenum distanceModel) = NULL;

ALCcontext* (ALC_APIENTRY *qalcCreateContext)(ALCdevice *device, const ALCint* attrlist) = NULL;
ALCboolean (ALC_APIENTRY *qalcMakeContextCurrent)(ALCcontext *context) = NULL;
void (ALC_APIENTRY *qalcProcessContext)(ALCcontext *context) = NULL;
void (ALC_APIENTRY *qalcSuspendContext)(ALCcontext *context) = NULL;
void (ALC_APIENTRY *qalcDestroyContext)(ALCcontext *context) = NULL;
ALCcontext* (ALC_APIENTRY *qalcGetCurrentContext)(void) = NULL;
ALCdevice* (ALC_APIENTRY *qalcGetContextsDevice)(ALCcontext *context) = NULL;
ALCdevice* (ALC_APIENTRY *qalcOpenDevice)(const ALCchar *devicename) = NULL;
ALCboolean (ALC_APIENTRY *qalcCloseDevice)(ALCdevice *device) = NULL;
ALCenum (ALC_APIENTRY *qalcGetError)(ALCdevice *device) = NULL;
ALCboolean (ALC_APIENTRY *qalcIsExtensionPresent)(ALCdevice *device, const ALCchar *extname) = NULL;
void* (ALC_APIENTRY *qalcGetProcAddress)(ALCdevice *device, const ALCchar *funcname) = NULL;
ALCenum (ALC_APIENTRY *qalcGetEnumValue)(ALCdevice *device, const ALCchar *enumname) = NULL;
const ALCchar* (ALC_APIENTRY *qalcGetString)(ALCdevice *device, ALCenum param) = NULL;
void (ALC_APIENTRY *qalcGetIntegerv)(ALCdevice *device, ALCenum param, ALCsizei size, ALCint *values) = NULL;

// Effect objects
LPALGENEFFECTS qalGenEffects = NULL;
LPALDELETEEFFECTS qalDeleteEffects = NULL;
LPALISEFFECT qalIsEffect = NULL;
LPALEFFECTI qalEffecti = NULL;
LPALEFFECTIV qalEffectiv = NULL;
LPALEFFECTF qalEffectf = NULL;
LPALEFFECTFV qalEffectfv = NULL;
LPALGETEFFECTI qalGetEffecti = NULL;
LPALGETEFFECTIV qalGetEffectiv = NULL;
LPALGETEFFECTF qalGetEffectf = NULL;
LPALGETEFFECTFV qalGetEffectfv = NULL;

//Filter objects
LPALGENFILTERS qalGenFilters = NULL;
LPALDELETEFILTERS qalDeleteFilters = NULL;
LPALISFILTER qalIsFilter = NULL;
LPALFILTERI qalFilteri = NULL;
LPALFILTERIV qalFilteriv = NULL;
LPALFILTERF qalFilterf = NULL;
LPALFILTERFV qalFilterfv = NULL;
LPALGETFILTERI qalGetFilteri = NULL;
LPALGETFILTERIV qalGetFilteriv = NULL;
LPALGETFILTERF qalGetFilterf = NULL;
LPALGETFILTERFV qalGetFilterfv = NULL;

// Auxiliary slot object
LPALGENAUXILIARYEFFECTSLOTS qalGenAuxiliaryEffectSlots = NULL;
LPALDELETEAUXILIARYEFFECTSLOTS qalDeleteAuxiliaryEffectSlots = NULL;
LPALISAUXILIARYEFFECTSLOT qalIsAuxiliaryEffectSlot = NULL;
LPALAUXILIARYEFFECTSLOTI qalAuxiliaryEffectSloti = NULL;
LPALAUXILIARYEFFECTSLOTIV qalAuxiliaryEffectSlotiv = NULL;
LPALAUXILIARYEFFECTSLOTF qalAuxiliaryEffectSlotf = NULL;
LPALAUXILIARYEFFECTSLOTFV qalAuxiliaryEffectSlotfv = NULL;
LPALGETAUXILIARYEFFECTSLOTI qalGetAuxiliaryEffectSloti = NULL;
LPALGETAUXILIARYEFFECTSLOTIV qalGetAuxiliaryEffectSlotiv = NULL;
LPALGETAUXILIARYEFFECTSLOTF qalGetAuxiliaryEffectSlotf = NULL;
LPALGETAUXILIARYEFFECTSLOTFV qalGetAuxiliaryEffectSlotfv = NULL;

qboolean qal_init = false;
qboolean qal_efxinit = false;

void QAL_InitEFXExtension(void);

void QAL_Init(void)
{
	HMODULE hOpenAL = LoadLibraryA("soft_oal.dll");
	if(!hOpenAL)
	{
		hOpenAL = LoadLibraryA("OpenAL32.dll");
	}

	if (hOpenAL)
	{
		qalEnable = (void (AL_APIENTRY *)(ALenum))GetProcAddress(hOpenAL, "alEnable");
		qalDisable = (void (AL_APIENTRY *)(ALenum))GetProcAddress(hOpenAL, "alDisable");
		qalIsEnabled = (ALboolean (AL_APIENTRY *)(ALenum))GetProcAddress(hOpenAL, "alIsEnabled");
		qalGetString = (const ALchar*(AL_APIENTRY *)(ALenum))GetProcAddress(hOpenAL, "alGetString");
		qalGetBooleanv = (void (AL_APIENTRY *)(ALenum, ALboolean*))GetProcAddress(hOpenAL, "alGetBooleanv");
		qalGetIntegerv = (void (AL_APIENTRY *)(ALenum, ALint*))GetProcAddress(hOpenAL, "alGetIntegerv");
		qalGetFloatv = (void (AL_APIENTRY *)(ALenum, ALfloat*))GetProcAddress(hOpenAL, "alGetFloatv");
		qalGetDoublev = (void (AL_APIENTRY *)(ALenum, ALdouble*))GetProcAddress(hOpenAL, "alGetDoublev");
		qalGetBoolean = (ALboolean (AL_APIENTRY *)(ALenum))GetProcAddress(hOpenAL, "alGetBoolean");
		qalGetInteger = (ALint (AL_APIENTRY *)(ALenum))GetProcAddress(hOpenAL, "alGetInteger");
		qalGetFloat = (ALfloat (AL_APIENTRY *)(ALenum))GetProcAddress(hOpenAL, "alGetFloat");
		qalGetDouble = (ALdouble (AL_APIENTRY *)(ALenum))GetProcAddress(hOpenAL, "alGetDouble");
		qalGetError = (ALenum (AL_APIENTRY *)())GetProcAddress(hOpenAL, "alGetError");
		qalIsExtensionPresent = (ALboolean (AL_APIENTRY *)(const ALchar*))GetProcAddress(hOpenAL, "alIsExtensionPresent");
		qalGetProcAddress = (void* (AL_APIENTRY *)(const ALchar*))GetProcAddress(hOpenAL, "alGetProcAddress");
		qalGetEnumValue = (ALenum (AL_APIENTRY *)(const ALchar*))GetProcAddress(hOpenAL, "alGetEnumValue");
		qalListenerf = (void (AL_APIENTRY *)(ALenum, ALfloat))GetProcAddress(hOpenAL, "alListenerf");
		qalListener3f = (void (AL_APIENTRY *)(ALenum, ALfloat, ALfloat, ALfloat))GetProcAddress(hOpenAL, "alListener3f");
		qalListenerfv = (void (AL_APIENTRY *)(ALenum, const ALfloat*))GetProcAddress(hOpenAL, "alListenerfv");
		qalListeneri = (void (AL_APIENTRY *)(ALenum, ALint))GetProcAddress(hOpenAL, "alListeneri");
		qalListener3i = (void (AL_APIENTRY *)(ALenum, ALint, ALint, ALint))GetProcAddress(hOpenAL, "alListener3i");
		qalListeneriv = (void (AL_APIENTRY *)(ALenum, const ALint*))GetProcAddress(hOpenAL, "alListeneriv");
		qalGetListenerf = (void (AL_APIENTRY *)(ALenum, ALfloat*))GetProcAddress(hOpenAL, "alGetListenerf");
		qalGetListener3f = (void (AL_APIENTRY *)(ALenum, ALfloat *, ALfloat *, ALfloat *))GetProcAddress(hOpenAL, "alGetListener3f");
		qalGetListenerfv = (void (AL_APIENTRY *)(ALenum, ALfloat*))GetProcAddress(hOpenAL, "alGetListenerfv");
		qalGetListeneri = (void (AL_APIENTRY *)(ALenum, ALint*))GetProcAddress(hOpenAL, "alGetListeneri");
		qalGetListener3i = (void (AL_APIENTRY *)(ALenum, ALint *, ALint *, ALint *))GetProcAddress(hOpenAL, "alGetListener3i");
		qalGetListeneriv = (void (AL_APIENTRY *)(ALenum, ALint*))GetProcAddress(hOpenAL, "alGetListeneriv");
		qalGenSources = (void (AL_APIENTRY *)(ALsizei, ALuint*))GetProcAddress(hOpenAL, "alGenSources");
		qalDeleteSources = (void (AL_APIENTRY *)(ALsizei, const ALuint*))GetProcAddress(hOpenAL, "alDeleteSources");
		qalIsSource = (ALboolean (AL_APIENTRY *)(ALuint))GetProcAddress(hOpenAL, "alIsSource");
		qalSourcef = (void (AL_APIENTRY *)(ALuint, ALenum, ALfloat))GetProcAddress(hOpenAL, "alSourcef");
		qalSource3f = (void (AL_APIENTRY *)(ALuint, ALenum, ALfloat, ALfloat, ALfloat))GetProcAddress(hOpenAL, "alSource3f");
		qalSourcefv = (void (AL_APIENTRY *)(ALuint, ALenum, const ALfloat*))GetProcAddress(hOpenAL, "alSourcefv");
		qalSourcei = (void (AL_APIENTRY *)(ALuint, ALenum, ALint))GetProcAddress(hOpenAL, "alSourcei");
		qalSource3i = (void (AL_APIENTRY *)(ALuint, ALenum, ALint, ALint, ALint))GetProcAddress(hOpenAL, "alSource3i");
		qalSourceiv = (void (AL_APIENTRY *)(ALuint, ALenum, const ALint*))GetProcAddress(hOpenAL, "alSourceiv");
		qalGetSourcef = (void (AL_APIENTRY *)(ALuint, ALenum, ALfloat*))GetProcAddress(hOpenAL, "alGetSourcef");
		qalGetSource3f = (void (AL_APIENTRY *)(ALuint, ALenum, ALfloat*, ALfloat*, ALfloat*))GetProcAddress(hOpenAL, "alGetSource3f");
		qalGetSourcefv = (void (AL_APIENTRY *)(ALuint, ALenum, ALfloat*))GetProcAddress(hOpenAL, "alGetSourcefv");
		qalGetSourcei = (void (AL_APIENTRY *)(ALuint, ALenum, ALint*))GetProcAddress(hOpenAL, "alGetSourcei");
		qalGetSource3i = (void (AL_APIENTRY *)(ALuint, ALenum, ALint*, ALint*, ALint*))GetProcAddress(hOpenAL, "alGetSource3i");
		qalGetSourceiv = (void (AL_APIENTRY *)(ALuint, ALenum, ALint*))GetProcAddress(hOpenAL, "alGetSourceiv");
		qalSourcePlayv = (void (AL_APIENTRY *)(ALsizei, const ALuint *))GetProcAddress(hOpenAL, "alSourcePlayv");
		qalSourceStopv = (void (AL_APIENTRY *)(ALsizei, const ALuint *))GetProcAddress(hOpenAL, "alSourceStopv");
		qalSourceRewindv = (void (AL_APIENTRY *)(ALsizei, const ALuint *))GetProcAddress(hOpenAL, "alSourceRewindv");
		qalSourcePausev = (void (AL_APIENTRY *)(ALsizei, const ALuint *))GetProcAddress(hOpenAL, "alSourcePausev");
		qalSourcePlay = (void (AL_APIENTRY *)(ALuint))GetProcAddress(hOpenAL, "alSourcePlay");
		qalSourceStop = (void (AL_APIENTRY *)(ALuint))GetProcAddress(hOpenAL, "alSourceStop");
		qalSourceRewind = (void (AL_APIENTRY *)(ALuint))GetProcAddress(hOpenAL, "alSourceRewind");
		qalSourcePause = (void (AL_APIENTRY *)(ALuint))GetProcAddress(hOpenAL, "alSourcePause");
		qalSourceQueueBuffers = (void (AL_APIENTRY *)(ALuint, ALsizei, const ALuint *))GetProcAddress(hOpenAL, "alSourceQueueBuffers");
		qalSourceUnqueueBuffers = (void (AL_APIENTRY *)(ALuint, ALsizei, ALuint *))GetProcAddress(hOpenAL, "alSourceUnqueueBuffers");
		qalGenBuffers = (void (AL_APIENTRY *)(ALsizei, ALuint*))GetProcAddress(hOpenAL, "alGenBuffers");
		qalDeleteBuffers = (void (AL_APIENTRY *)(ALsizei, const ALuint*))GetProcAddress(hOpenAL, "alDeleteBuffers");
		qalIsBuffer = (ALboolean (AL_APIENTRY *)(ALuint))GetProcAddress(hOpenAL, "alIsBuffer");
		qalBufferData = (void (AL_APIENTRY *)(ALuint, ALenum, const ALvoid*, ALsizei, ALsizei))GetProcAddress(hOpenAL, "alBufferData");
		qalBufferf = (void (AL_APIENTRY *)(ALuint, ALenum, ALfloat))GetProcAddress(hOpenAL, "alBufferf");
		qalBuffer3f = (void (AL_APIENTRY *)(ALuint, ALenum, ALfloat, ALfloat, ALfloat))GetProcAddress(hOpenAL, "alBuffer3f");
		qalBufferfv = (void (AL_APIENTRY *)(ALuint, ALenum, const ALfloat*))GetProcAddress(hOpenAL, "alBufferfv");
		qalBufferi = (void (AL_APIENTRY *)(ALuint, ALenum, ALint))GetProcAddress(hOpenAL, "alBufferi");
		qalBuffer3i = (void (AL_APIENTRY *)(ALuint, ALenum, ALint, ALint, ALint))GetProcAddress(hOpenAL, "alBuffer3i");
		qalBufferiv = (void (AL_APIENTRY *)(ALuint, ALenum, const ALint*))GetProcAddress(hOpenAL, "alBufferiv");
		qalGetBufferf = (void (AL_APIENTRY *)(ALuint, ALenum, ALfloat*))GetProcAddress(hOpenAL, "alGetBufferf");
		qalGetBuffer3f = (void (AL_APIENTRY *)(ALuint, ALenum, ALfloat*, ALfloat*, ALfloat*))GetProcAddress(hOpenAL, "alGetBuffer3f");
		qalGetBufferfv = (void (AL_APIENTRY *)(ALuint, ALenum, ALfloat*))GetProcAddress(hOpenAL, "alGetBufferfv");
		qalGetBufferi = (void (AL_APIENTRY *)(ALuint, ALenum, ALint*))GetProcAddress(hOpenAL, "alGetBufferi");
		qalGetBuffer3i = (void (AL_APIENTRY *)(ALuint, ALenum, ALint*, ALint*, ALint*))GetProcAddress(hOpenAL, "alGetBuffer3i");
		qalGetBufferiv = (void (AL_APIENTRY *)(ALuint, ALenum, ALint*))GetProcAddress(hOpenAL, "alGetBufferiv");
		qalDopplerFactor = (void (AL_APIENTRY *)(ALfloat))GetProcAddress(hOpenAL, "alDopplerFactor");
		qalDopplerVelocity = (void (AL_APIENTRY *)(ALfloat))GetProcAddress(hOpenAL, "alDopplerVelocity");
		qalSpeedOfSound = (void (AL_APIENTRY *)(ALfloat))GetProcAddress(hOpenAL, "alSpeedOfSound");
		qalDistanceModel = (void (AL_APIENTRY *)(ALenum))GetProcAddress(hOpenAL, "alDistanceModel");

		qalcCreateContext = (ALCcontext* (ALC_APIENTRY *)(ALCdevice *, const ALCint*))GetProcAddress(hOpenAL, "alcCreateContext");
		qalcMakeContextCurrent = (ALCboolean (ALC_APIENTRY *)(ALCcontext *))GetProcAddress(hOpenAL, "alcMakeContextCurrent");
		qalcProcessContext = (void (ALC_APIENTRY *)(ALCcontext *))GetProcAddress(hOpenAL, "alcProcessContext");
		qalcSuspendContext = (void (ALC_APIENTRY *)(ALCcontext *))GetProcAddress(hOpenAL, "alcSuspendContext");
		qalcDestroyContext = (void (ALC_APIENTRY *)(ALCcontext *))GetProcAddress(hOpenAL, "alcDestroyContext");
		qalcGetCurrentContext = (ALCcontext* (ALC_APIENTRY *)())GetProcAddress(hOpenAL, "alcGetCurrentContext");
		qalcGetContextsDevice = (ALCdevice* (ALC_APIENTRY *)(ALCcontext *))GetProcAddress(hOpenAL, "alcGetContextsDevice");
		qalcOpenDevice = (ALCdevice* (ALC_APIENTRY *)(const ALCchar *))GetProcAddress(hOpenAL, "alcOpenDevice");
		qalcCloseDevice = (ALCboolean (ALC_APIENTRY *)(ALCdevice *))GetProcAddress(hOpenAL, "alcCloseDevice");
		qalcGetError = (ALCenum (ALC_APIENTRY *)(ALCdevice *))GetProcAddress(hOpenAL, "alcGetError");
		qalcIsExtensionPresent = (ALCboolean (ALC_APIENTRY *)(ALCdevice *, const ALCchar *))GetProcAddress(hOpenAL, "alcIsExtensionPresent");
		qalcGetProcAddress = (void* (ALC_APIENTRY *)(ALCdevice *, const ALCchar *))GetProcAddress(hOpenAL, "alcGetProcAddress");
		qalcGetEnumValue = (ALCenum (ALC_APIENTRY *)(ALCdevice *, const ALCchar *))GetProcAddress(hOpenAL, "alcGetEnumValue");
		qalcGetString = (const ALCchar* (ALC_APIENTRY *)(ALCdevice *, ALCenum))GetProcAddress(hOpenAL, "alcGetString");
		qalcGetIntegerv = (void (ALC_APIENTRY *)(ALCdevice *, ALCenum, ALCsizei , ALCint *))GetProcAddress(hOpenAL, "alcGetIntegerv");
	
		qal_init = true;
	}
}

void QAL_InitEFXExtension(void)
{
	qal_efxinit = false;

	if(!qal_init)
		return;

	ALCdevice *pDevice = NULL;
	ALCcontext *pContext = NULL;
	ALboolean bEFXSupport = AL_FALSE;

	pContext = qalcGetCurrentContext();
	pDevice = qalcGetContextsDevice(pContext);

	if (qalcIsExtensionPresent(pDevice, (ALCchar*)ALC_EXT_EFX_NAME))
	{
		// Get function pointers
		qalGenEffects = (LPALGENEFFECTS)qalGetProcAddress("alGenEffects");
		qalDeleteEffects = (LPALDELETEEFFECTS )qalGetProcAddress("alDeleteEffects");
		qalIsEffect = (LPALISEFFECT )qalGetProcAddress("alIsEffect");
		qalEffecti = (LPALEFFECTI)qalGetProcAddress("alEffecti");
		qalEffectiv = (LPALEFFECTIV)qalGetProcAddress("alEffectiv");
		qalEffectf = (LPALEFFECTF)qalGetProcAddress("alEffectf");
		qalEffectfv = (LPALEFFECTFV)qalGetProcAddress("alEffectfv");
		qalGetEffecti = (LPALGETEFFECTI)qalGetProcAddress("alGetEffecti");
		qalGetEffectiv = (LPALGETEFFECTIV)qalGetProcAddress("alGetEffectiv");
		qalGetEffectf = (LPALGETEFFECTF)qalGetProcAddress("alGetEffectf");
		qalGetEffectfv = (LPALGETEFFECTFV)qalGetProcAddress("alGetEffectfv");
		qalGenFilters = (LPALGENFILTERS)qalGetProcAddress("alGenFilters");
		qalDeleteFilters = (LPALDELETEFILTERS)qalGetProcAddress("alDeleteFilters");
		qalIsFilter = (LPALISFILTER)qalGetProcAddress("alIsFilter");
		qalFilteri = (LPALFILTERI)qalGetProcAddress("alFilteri");
		qalFilteriv = (LPALFILTERIV)qalGetProcAddress("alFilteriv");
		qalFilterf = (LPALFILTERF)qalGetProcAddress("alFilterf");
		qalFilterfv = (LPALFILTERFV)qalGetProcAddress("alFilterfv");
		qalGetFilteri = (LPALGETFILTERI)qalGetProcAddress("alGetFilteri");
		qalGetFilteriv= (LPALGETFILTERIV)qalGetProcAddress("alGetFilteriv");
		qalGetFilterf = (LPALGETFILTERF)qalGetProcAddress("alGetFilterf");
		qalGetFilterfv= (LPALGETFILTERFV)qalGetProcAddress("alGetFilterfv");
		qalGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)qalGetProcAddress("alGenAuxiliaryEffectSlots");
		qalDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)qalGetProcAddress("alDeleteAuxiliaryEffectSlots");
		qalIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)qalGetProcAddress("alIsAuxiliaryEffectSlot");
		qalAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)qalGetProcAddress("alAuxiliaryEffectSloti");
		qalAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)qalGetProcAddress("alAuxiliaryEffectSlotiv");
		qalAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)qalGetProcAddress("alAuxiliaryEffectSlotf");
		qalAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)qalGetProcAddress("alAuxiliaryEffectSlotfv");
		qalGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)qalGetProcAddress("alGetAuxiliaryEffectSloti");
		qalGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)qalGetProcAddress("alGetAuxiliaryEffectSlotiv");
		qalGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)qalGetProcAddress("alGetAuxiliaryEffectSlotf");
		qalGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)qalGetProcAddress("alGetAuxiliaryEffectSlotfv");

		if (qalGenEffects && qalDeleteEffects && qalIsEffect && qalEffecti && qalEffectiv && qalEffectf &&
			qalEffectfv && qalGetEffecti && qalGetEffectiv && qalGetEffectf && qalGetEffectfv && qalGenFilters &&
			qalDeleteFilters && qalIsFilter && qalFilteri && qalFilteriv && qalFilterf && qalFilterfv &&
			qalGetFilteri && qalGetFilteriv && qalGetFilterf && qalGetFilterfv && qalGenAuxiliaryEffectSlots &&
			qalDeleteAuxiliaryEffectSlots && qalIsAuxiliaryEffectSlot && qalAuxiliaryEffectSloti &&
			qalAuxiliaryEffectSlotiv && qalAuxiliaryEffectSlotf && qalAuxiliaryEffectSlotfv &&
			qalGetAuxiliaryEffectSloti && qalGetAuxiliaryEffectSlotiv && qalGetAuxiliaryEffectSlotf &&
			qalGetAuxiliaryEffectSlotfv)
			bEFXSupport = AL_TRUE;
	}

	qal_efxinit = bEFXSupport;
}