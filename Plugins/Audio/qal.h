#ifndef QAL_H
#define QAL_H

#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "OpenAL/efx.h"

extern void (AL_APIENTRY *qalEnable)(ALenum capability);
extern void (AL_APIENTRY *qalDisable)(ALenum capability);
extern ALboolean (AL_APIENTRY *qalIsEnabled)(ALenum capability);
extern const ALchar* (AL_APIENTRY *qalGetString)(ALenum param);
extern void (AL_APIENTRY *qalGetBooleanv)(ALenum param, ALboolean* data);
extern void (AL_APIENTRY *qalGetIntegerv)(ALenum param, ALint* data);
extern void (AL_APIENTRY *qalGetFloatv)(ALenum param, ALfloat* data);
extern void (AL_APIENTRY *qalGetDoublev)(ALenum param, ALdouble* data);
extern ALboolean (AL_APIENTRY *qalGetBoolean)(ALenum param);
extern ALint (AL_APIENTRY *qalGetInteger)(ALenum param);
extern ALfloat (AL_APIENTRY *qalGetFloat)(ALenum param);
extern ALdouble (AL_APIENTRY *qalGetDouble)(ALenum param);
extern ALenum (AL_APIENTRY *qalGetError)(void);
extern ALboolean (AL_APIENTRY *qalIsExtensionPresent)(const ALchar* extname);
extern void* (AL_APIENTRY *qalGetProcAddress)(const ALchar* fname);
extern ALenum (AL_APIENTRY *qalGetEnumValue)(const ALchar* ename);
extern void (AL_APIENTRY *qalListenerf)(ALenum param, ALfloat value);
extern void (AL_APIENTRY *qalListener3f)(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
extern void (AL_APIENTRY *qalListenerfv)(ALenum param, const ALfloat* values);
extern void (AL_APIENTRY *qalListeneri)(ALenum param, ALint value);
extern void (AL_APIENTRY *qalListener3i)(ALenum param, ALint value1, ALint value2, ALint value3);
extern void (AL_APIENTRY *qalListeneriv)(ALenum param, const ALint* values);
extern void (AL_APIENTRY *qalGetListenerf)(ALenum param, ALfloat* value);
extern void (AL_APIENTRY *qalGetListener3f)(ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
extern void (AL_APIENTRY *qalGetListenerfv)(ALenum param, ALfloat* values);
extern void (AL_APIENTRY *qalGetListeneri)(ALenum param, ALint* value);
extern void (AL_APIENTRY *qalGetListener3i)(ALenum param, ALint *value1, ALint *value2, ALint *value3);
extern void (AL_APIENTRY *qalGetListeneriv)(ALenum param, ALint* values);
extern void (AL_APIENTRY *qalGenSources)(ALsizei n, ALuint* sources);
extern void (AL_APIENTRY *qalDeleteSources)(ALsizei n, const ALuint* sources);
extern ALboolean (AL_APIENTRY *qalIsSource)(ALuint sid);
extern void (AL_APIENTRY *qalSourcef)(ALuint sid, ALenum param, ALfloat value);
extern void (AL_APIENTRY *qalSource3f)(ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
extern void (AL_APIENTRY *qalSourcefv)(ALuint sid, ALenum param, const ALfloat* values);
extern void (AL_APIENTRY *qalSourcei)(ALuint sid, ALenum param, ALint value);
extern void (AL_APIENTRY *qalSource3i)(ALuint sid, ALenum param, ALint value1, ALint value2, ALint value3);
extern void (AL_APIENTRY *qalSourceiv)(ALuint sid, ALenum param, const ALint* values);
extern void (AL_APIENTRY *qalGetSourcef)(ALuint sid, ALenum param, ALfloat* value);
extern void (AL_APIENTRY *qalGetSource3f)(ALuint sid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
extern void (AL_APIENTRY *qalGetSourcefv)(ALuint sid, ALenum param, ALfloat* values);
extern void (AL_APIENTRY *qalGetSourcei)(ALuint sid,  ALenum param, ALint* value);
extern void (AL_APIENTRY *qalGetSource3i)(ALuint sid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
extern void (AL_APIENTRY *qalGetSourceiv)(ALuint sid,  ALenum param, ALint* values);
extern void (AL_APIENTRY *qalSourcePlayv)(ALsizei ns, const ALuint *sids);
extern void (AL_APIENTRY *qalSourceStopv)(ALsizei ns, const ALuint *sids);
extern void (AL_APIENTRY *qalSourceRewindv)(ALsizei ns, const ALuint *sids);
extern void (AL_APIENTRY *qalSourcePausev)(ALsizei ns, const ALuint *sids);
extern void (AL_APIENTRY *qalSourcePlay)(ALuint sid);
extern void (AL_APIENTRY *qalSourceStop)(ALuint sid);
extern void (AL_APIENTRY *qalSourceRewind)(ALuint sid);
extern void (AL_APIENTRY *qalSourcePause)(ALuint sid);
extern void (AL_APIENTRY *qalSourceQueueBuffers)(ALuint sid, ALsizei numEntries, const ALuint *bids);
extern void (AL_APIENTRY *qalSourceUnqueueBuffers)(ALuint sid, ALsizei numEntries, ALuint *bids);
extern void (AL_APIENTRY *qalGenBuffers)(ALsizei n, ALuint* buffers);
extern void (AL_APIENTRY *qalDeleteBuffers)(ALsizei n, const ALuint* buffers);
extern ALboolean (AL_APIENTRY *qalIsBuffer)(ALuint bid);
extern void (AL_APIENTRY *qalBufferData)(ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq);
extern void (AL_APIENTRY *qalBufferf)(ALuint bid, ALenum param, ALfloat value);
extern void (AL_APIENTRY *qalBuffer3f)(ALuint bid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
extern void (AL_APIENTRY *qalBufferfv)(ALuint bid, ALenum param, const ALfloat* values);
extern void (AL_APIENTRY *qalBufferi)(ALuint bid, ALenum param, ALint value);
extern void (AL_APIENTRY *qalBuffer3i)(ALuint bid, ALenum param, ALint value1, ALint value2, ALint value3);
extern void (AL_APIENTRY *qalBufferiv)(ALuint bid, ALenum param, const ALint* values);
extern void (AL_APIENTRY *qalGetBufferf)(ALuint bid, ALenum param, ALfloat* value);
extern void (AL_APIENTRY *qalGetBuffer3f)(ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
extern void (AL_APIENTRY *qalGetBufferfv)(ALuint bid, ALenum param, ALfloat* values);
extern void (AL_APIENTRY *qalGetBufferi)(ALuint bid, ALenum param, ALint* value);
extern void (AL_APIENTRY *qalGetBuffer3i)(ALuint bid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
extern void (AL_APIENTRY *qalGetBufferiv)(ALuint bid, ALenum param, ALint* values);
extern void (AL_APIENTRY *qalDopplerFactor)(ALfloat value);
extern void (AL_APIENTRY *qalDopplerVelocity)(ALfloat value);
extern void (AL_APIENTRY *qalSpeedOfSound)(ALfloat value);
extern void (AL_APIENTRY *qalDistanceModel)(ALenum distanceModel);

extern ALCcontext* (ALC_APIENTRY *qalcCreateContext)(ALCdevice *device, const ALCint* attrlist);
extern ALCboolean (ALC_APIENTRY *qalcMakeContextCurrent)(ALCcontext *context);
extern void (ALC_APIENTRY *qalcProcessContext)(ALCcontext *context);
extern void (ALC_APIENTRY *qalcSuspendContext)(ALCcontext *context);
extern void (ALC_APIENTRY *qalcDestroyContext)(ALCcontext *context);
extern ALCcontext* (ALC_APIENTRY *qalcGetCurrentContext)(void);
extern ALCdevice* (ALC_APIENTRY *qalcGetContextsDevice)(ALCcontext *context);
extern ALCdevice* (ALC_APIENTRY *qalcOpenDevice)(const ALCchar *devicename);
extern ALCboolean (ALC_APIENTRY *qalcCloseDevice)(ALCdevice *device);
extern ALCenum (ALC_APIENTRY *qalcGetError)(ALCdevice *device);
extern ALCboolean (ALC_APIENTRY *qalcIsExtensionPresent)(ALCdevice *device, const ALCchar *extname);
extern void* (ALC_APIENTRY *qalcGetProcAddress)(ALCdevice *device, const ALCchar *funcname);
extern ALCenum (ALC_APIENTRY *qalcGetEnumValue)(ALCdevice *device, const ALCchar *enumname);
extern const ALCchar* (ALC_APIENTRY *qalcGetString)(ALCdevice *device, ALCenum param);
extern void (ALC_APIENTRY *qalcGetIntegerv)(ALCdevice *device, ALCenum param, ALCsizei size, ALCint *values);

// Effect objects
extern LPALGENEFFECTS qalGenEffects;
extern LPALDELETEEFFECTS qalDeleteEffects;
extern LPALISEFFECT qalIsEffect;
extern LPALEFFECTI qalEffecti;
extern LPALEFFECTIV qalEffectiv;
extern LPALEFFECTF qalEffectf;
extern LPALEFFECTFV qalEffectfv;
extern LPALGETEFFECTI qalGetEffecti;
extern LPALGETEFFECTIV qalGetEffectiv;
extern LPALGETEFFECTF qalGetEffectf;
extern LPALGETEFFECTFV qalGetEffectfv;

//Filter objects
extern LPALGENFILTERS qalGenFilters;
extern LPALDELETEFILTERS qalDeleteFilters;
extern LPALISFILTER qalIsFilter;
extern LPALFILTERI qalFilteri;
extern LPALFILTERIV qalFilteriv;
extern LPALFILTERF qalFilterf;
extern LPALFILTERFV qalFilterfv;
extern LPALGETFILTERI qalGetFilteri;
extern LPALGETFILTERIV qalGetFilteriv;
extern LPALGETFILTERF qalGetFilterf;
extern LPALGETFILTERFV qalGetFilterfv;

// Auxiliary slot object
extern LPALGENAUXILIARYEFFECTSLOTS qalGenAuxiliaryEffectSlots;
extern LPALDELETEAUXILIARYEFFECTSLOTS qalDeleteAuxiliaryEffectSlots;
extern LPALISAUXILIARYEFFECTSLOT qalIsAuxiliaryEffectSlot;
extern LPALAUXILIARYEFFECTSLOTI qalAuxiliaryEffectSloti;
extern LPALAUXILIARYEFFECTSLOTIV qalAuxiliaryEffectSlotiv;
extern LPALAUXILIARYEFFECTSLOTF qalAuxiliaryEffectSlotf;
extern LPALAUXILIARYEFFECTSLOTFV qalAuxiliaryEffectSlotfv;
extern LPALGETAUXILIARYEFFECTSLOTI qalGetAuxiliaryEffectSloti;
extern LPALGETAUXILIARYEFFECTSLOTIV qalGetAuxiliaryEffectSlotiv;
extern LPALGETAUXILIARYEFFECTSLOTF qalGetAuxiliaryEffectSlotf;
extern LPALGETAUXILIARYEFFECTSLOTFV qalGetAuxiliaryEffectSlotfv;

void QAL_Init(void);
void QAL_InitEFXExtension(void);

extern qboolean qal_init;
extern qboolean qal_efxinit;

#endif