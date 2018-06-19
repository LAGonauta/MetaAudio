#pragma once

#include "enginedef.h"
#include "aud_int_internal.h"
#include "alure/AL/alure2.h"

//internal structures

typedef struct
{
  sfx_t *sfx;
  alure::Buffer buffer;
  alure::Source source;
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
}aud_channel_t;

typedef struct
{
  //wave info
  int 	length;
  int 	loopstart;
  int 	speed;
  int 	width;
  int 	channels;
  int		dataofs;
  int		bitrate;
  int		blockalign;
  //for OpenAL buffer
  bool alstreaming;
  char alpath[MAX_PATH];
  //for Stream sound
  FileHandle_t file;
  int		filesize;
  //data chunk so we could do some magic change on the raw sound data
  int		datalen;
  std::vector<byte>	data;
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
qboolean GetWavinfo(wavinfo_t *info, char *name, byte *wav, int wavlength);
void ResampleSfx(aud_sfxcache_t *sc, byte *indata, byte *outdata);

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
void VoiceSE_QueueBuffers(aud_channel_t *ch);
aud_sfxcache_t *VoiceSE_GetSFXCache(sfx_t *s, aud_channel_t *ch);
void VoiceSE_NotifyFreeChannel(aud_channel_t *ch);

//snd_fx.cpp
void SX_RoomFX(void);
void SX_Init(void);
void SX_Shutdown(void);
void SX_ApplyEffect(aud_channel_t *ch, int roomtype, qboolean underwater);

//common
extern aud_engine_t gAudEngine;

//active control
extern cvar_t *al_enable;
extern qboolean openal_started;
extern qboolean openal_enabled;