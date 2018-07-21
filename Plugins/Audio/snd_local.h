#pragma once

#include "enginedef.h"
#include "aud_int_internal.h"
#include "alure/AL/alure2.h"

//internal structures

typedef struct
{
  sfx_t *sfx;
  float volume;
  float pitch;
  float attenuation;
  int entnum;
  int entchannel;
  vec3_t origin;
  int start;
  int end;
  //for sentence
  int	isentence;
  int	iword;
  //for voice sound
  sfxcache_t *voicecache;

  // For OpenAL
  alure::SharedPtr<alure::Decoder> decoder;
  alure::SharedPtr<alure::Buffer> buffer;
  alure::Source source;

  // For OpenAL EFX
  bool firstpass;            // true if this is first time sound is spatialized
  float ob_gain;             // gain drop if sound source obscured from listener
  float ob_gain_target;      // target gain while crossfading between ob_gain & ob_gain_target
  float ob_gain_inc;         // crossfade increment
}aud_channel_t;

typedef struct
{
  //wave info
  int length;
  int loopstart;
  int loopend;
  int samplerate;
  int width;
  alure::ChannelConfig channels;

  //OpenAL buffer
  alure::SharedPtr<alure::Decoder> decoder;
  alure::SharedPtr<alure::Buffer> buffer;

  alure::UniquePtr<alure::Vector<ALubyte>> data;
}aud_sfxcache_t;

typedef struct
{
  int *cl_servercount;
  int *cl_parsecount;
  int *cl_viewentity;
  int *cl_num_entities;
  int *cl_waterlevel;

  double *cl_time;
  double *cl_oldtime;

  float *g_SND_VoiceOverdrive;

  char *(*rgpszrawsentence)[CVOXFILESENTENCEMAX];
  int *cszrawsentences;

  //s_dma.c
  void(*S_Startup)(void);//hooked
  void(*S_Init)(void);//hooked
  void(*S_Shutdown)(void);//hooked	
  sfx_t *(*S_FindName)(char *name, int *pfInCache);//hooked
  sfx_t *(*S_PrecacheSound)(char *name);//hooked
  void(*SND_Spatialize)(aud_channel_t *ch);
  void(*S_Update)(float *origin, float *forward, float *right, float *up);//hooked
  void(*S_StartDynamicSound)(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch);//hooked
  void(*S_StartStaticSound)(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch);//hooked
  void(*S_StopSound)(int entnum, int entchannel);//hooked
  void(*S_StopAllSounds)(qboolean clear);//hooked

  //s_mem.c
  aud_sfxcache_t *(*S_LoadSound)(sfx_t *s, aud_channel_t *ch);//hooked

  //s_mix.c
  void(*VOX_Shutdown)(void);
  void(*S_FreeChannel)(channel_t *ch);

  //voice_sound_engine_interface.cpp
  void(*VoiceSE_NotifyFreeChannel)(int iChannel);
  void(*VoiceSE_Idle)(float frametime);

  //sequence.c
  sentenceEntry_s*(*SequenceGetSentenceByIndex)(unsigned int index);

  //sys_dll.c
#ifdef _DEBUG
  void(*Sys_Error)(char *fmt, ...);
#endif
}aud_engine_t;

//snd_hook.cpp
void S_FillAddress(void);
void S_InstallHook(void);

//snd_dma.cpp
void S_Startup(void);
void S_Init(void);
void S_Shutdown(void);
void S_ShutdownAL(void);
sfx_t *S_FindName(char *name, int *pfInCache);
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch);
void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch);
void S_StopSound(int entnum, int entchannel);
void S_StopAllSounds(qboolean clear);
void S_Update(float *origin, float *forward, float *right, float *up);
void S_FreeChannel(aud_channel_t *ch);
void S_FreeCache(sfx_t *sfx);
void S_FlushCaches(void);
qboolean SND_IsPlaying(sfx_t *sfx);

//snd_wav.cpp
class LocalAudioDecoder final : public alure::MessageHandler {
public:
  static const alure::Array<alure::String, 3> SupportedExtensions;
  bool GetWavinfo(wavinfo_t *info, char *full_path, alure::ArrayView<ALbyte>& data_output);
  void bufferLoading(alure::StringView name, alure::ChannelConfig channels, alure::SampleType type, ALuint samplerate, alure::ArrayView<ALbyte> data) noexcept override;

private:
  // To return the data to the application we copy the information here
  alure::StringView _name;
  alure::ChannelConfig _channels;
  alure::SampleType _type;
  ALuint _samplerate;
  alure::ArrayView<ALbyte> _data;
};

//snd_vox.cpp
void VOX_Init(void);
void VOX_Shutdown(void);
void VOX_TrimStartEndTimes(aud_channel_t *ch, aud_sfxcache_t *sc);
void VOX_SetChanVolPitch(aud_channel_t *ch, float *fvol, float *fpitch);
void VOX_ReadSentenceFile(void);
aud_sfxcache_t *VOX_LoadSound(aud_channel_t *pchan, char *pszin);
void VOX_MakeSingleWordSentence(aud_channel_t *ch, int pitch);
void SND_InitMouth(int entnum, int entchannel);
void SND_CloseMouth(aud_channel_t *ch);
void SND_MoveMouth(aud_channel_t *ch, aud_sfxcache_t *sc);

//snd_mem.cpp
aud_sfxcache_t *S_LoadSound(sfx_t *s, aud_channel_t *ch);
qboolean S_StreamLoadNextChunk(aud_channel_t *ch, aud_sfxcache_t *sc, ALuint alBuffer);

//snd_voice.cpp
class VoiceDecoder final : public alure::Decoder
{
  int(*VoiceSE_GetSoundDataCallback)(sfxcache_s *pCache, char *pCopyBuf, int maxOutDataSize, int samplePos, int sampleCount);

  alure::ChannelConfig m_channel_config{ alure::ChannelConfig::Mono };
  alure::SampleType m_sample_type{ alure::SampleType::UInt8 };
  size_t m_sample_rate{ 8000 };

  int m_entchannel;
  sfxcache_t *m_sfxcache_t;

public:
  VoiceDecoder(sfx_t *sound, aud_channel_t *ch);
  ~VoiceDecoder() override;

  ALuint getFrequency() const noexcept override;
  alure::ChannelConfig getChannelConfig() const noexcept override;
  alure::SampleType getSampleType() const noexcept override;
  
  bool hasLoopPoints() const noexcept override;
  std::pair<uint64_t, uint64_t> getLoopPoints() const noexcept override;

  uint64_t getLength() const noexcept override;
  bool seek(uint64_t pos) noexcept override;
  ALuint read(ALvoid *ptr, ALuint count) noexcept override;
};

//snd_fx.cpp
void SX_RoomFX(void);
void SX_Init(void);
void SX_Shutdown(void);
void SX_InterplEffect(int roomtype);
void SX_ApplyEffect(aud_channel_t *ch, qboolean underwater);

//common
extern aud_engine_t gAudEngine;

//active control
extern cvar_t *al_enable;
extern qboolean openal_started;
extern qboolean openal_enabled;