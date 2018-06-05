#include <metahook.h>
#include "plugins.h"
#include "exportfuncs.h"
#include "snd_local.h"

aud_engine_t gAudEngine;

#define GetCallAddress(addr) (addr + (*(DWORD *)((addr)+1)) + 5)
#define Sig_NotFound(name) Sys_ErrorEx("Could not found: %s\nEngine buildnum£º%d", #name, g_dwEngineBuildnum);
#define Sig_FuncNotFound(name) if(!gAudEngine.name) Sig_NotFound(name)
#define Sig_AddrNotFound(name) if(!addr) Sig_NotFound(name)

#define Sig_Length(a) (sizeof(a)-1)
#define Search_Pattern(sig) g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, sig, Sig_Length(sig));
#define Search_Pattern_From(func, sig) g_pMetaHookAPI->SearchPattern((void *)gAudEngine.func, g_dwEngineSize - (DWORD)gAudEngine.func + g_dwEngineBase, sig, Sig_Length(sig));
#define InstallHook(func) g_pMetaHookAPI->InlineHook((void *)gAudEngine.func, func, (void *&)gAudEngine.func);

//Signatures for 6153
#define S_INIT_SIG_NEW "\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08\x85\xC0"
#define S_SHUTDOWN_SIG_NEW "\xE8\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC9"
#define S_FINDNAME_SIG_NEW "\x55\x8B\xEC\x53\x56\x8B\x75\x08\x33\xDB\x85\xF6"
#define S_PRECACHESOUND_SIG_NEW "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x56\x85\xC0\x74\x2A\xD9\x05"
#define SND_SPATIALIZE_SIG_NEW "\x55\x8B\xEC\x83\xEC\x1C\x8B\x0D\x2A\x2A\x2A\x2A\x56"
#define S_STARTDYNAMICSOUND_SIG_NEW "\x55\x8B\xEC\x83\xEC\x48\xA1\x2A\x2A\x2A\x2A\x53\x56\x57\x85\xC0\xC7\x45\xFC\x00\x00\x00\x00"
#define S_STARTSTATICSOUND_SIG_NEW "\x55\x8B\xEC\x83\xEC\x44\x53\x56\x57\x8B\x7D\x10\x85\xFF\xC7\x45\xFC\x00\x00\x00\x00"
#define S_STOPSOUND_SIG_NEW "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x57\xBF\x04\x00\x00\x00\x3B\xC7"
#define S_STOPALLSOUNDS_SIG_NEW "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x85\xC0\x74\x4F\x56\xC7\x05"
#define S_UPDATE_SIG_NEW "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x85\xC0\x0F\x84\x2A\x2A\x00\x00\xA1\x2A\x2A\x2A\x2A\x85\xC0\x0F\x8F\x2A\x2A\x00\x00"
#define S_LOADSOUND_SIG_NEW "\x55\x8B\xEC\x81\xEC\x44\x05\x00\x00\x53\x56\x8B\x75\x08"
#define SEQUENCE_GETSENTENCEBYINDEX_SIG_NEW "\x55\x8B\xEC\x56\x8B\x35\x2A\x2A\x2A\x2A\x85\xF6\x57\x74\x2A\x8B\x7D\x08\x8B\x06\x57\x50\xE8"
#define VOICESE_IDLE_SIG_NEW "\x55\x8B\xEC\xA0\x2A\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\x84\xC0\x74\x2A\xD8\x1D"
#ifdef _DEBUG
#define SYS_ERROR_SIG_NEW "\x55\x8B\xEC\x81\xEC\x00\x04\x00\x00\x8B\x4D\x08\x8D\x45\x0C\x50\x51\x8D\x95\x00\xFC\xFF\xFF"
#endif

//Signatures for 3266
#define S_INIT_SIG "\x83\xEC\x08\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08\x85\xC0"
#define S_SHUTDOWN_SIG S_SHUTDOWN_SIG_NEW
#define S_FINDNAME_SIG "\x53\x55\x8B\x6C\x24\x0C\x33\xDB\x56\x57\x85\xED"
#define S_PRECACHESOUND_SIG "\xA1\x2A\x2A\x2A\x2A\x56\x85\xC0\x74\x2A\xD9\x05"
#define SND_SPATIALIZE_SIG "\x83\xEC\x34\x8B\x0D\x2A\x2A\x2A\x2A\x53\x56"
#define S_STARTDYNAMICSOUND_SIG "\x83\xEC\x48\xA1\x2A\x2A\x2A\x2A\x53\x55\x56\x85\xC0\x57\xC7\x44\x24\x10\x00\x00\x00\x00"
#define S_STARTSTATICSOUND_SIG "\x83\xEC\x44\x53\x55\x8B\x6C\x24\x58\x56\x85\xED\x57"
#define S_STOPSOUND_SIG "\xA1\x2A\x2A\x2A\x2A\x57\xBF\x04\x00\x00\x00\x3B\xC7"
#define S_STOPALLSOUNDS_SIG "\xA1\x2A\x2A\x2A\x2A\x85\xC0\x74\x2A\x56\xC7\x05"
#define S_UPDATE_SIG "\xA1\x2A\x2A\x2A\x2A\x85\xC0\x0F\x84\x2A\x2A\x00\x00\xA1\x2A\x2A\x2A\x2A\x85\xC0\x0F\x8F\x2A\x2A\x00\x00"
#define S_LOADSOUND_SIG "\x81\xEC\x2A\x2A\x00\x00\x53\x8B\x9C\x24\x2A\x2A\x00\x00\x55\x56\x8A\x03\x57"
#define SEQUENCE_GETSENTENCEBYINDEX_SIG "\x56\x8B\x35\x2A\x2A\x2A\x2A\x85\xF6\x57\x74\x2A\x8B\x7C\x24\x0C\x8B\x06\x57\x50\xE8"
#define VOICESE_IDLE_SIG "\xA0\x2A\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\x84\xC0\x74\x2A\xD8\x1D"
#ifdef _DEBUG
#define SYS_ERROR_SIG "\x8B\x4C\x24\x04\x81\xEC\x00\x04\x00\x00\x8D\x84\x24\x08\x04\x00\x00\x8D\x54\x24\x00\x50\x51\x68\x00\x04\x00\x00\x52\xE8\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x83\xC4\x10\x85\xC0"
#endif

void S_FillAddress(void)
{
	memset(&gAudEngine, 0, sizeof(gAudEngine));

	if(g_dwEngineBuildnum >= 5953)
	{
		gAudEngine.S_Init = (void (*)(void))Search_Pattern(S_INIT_SIG_NEW);
		Sig_FuncNotFound(S_Init);

		gAudEngine.S_Shutdown = (void (*)(void))Search_Pattern_From(S_Init, S_SHUTDOWN_SIG_NEW);
		Sig_FuncNotFound(S_Shutdown);

		gAudEngine.S_FindName = (sfx_t *(*)(char *, int *))Search_Pattern_From(S_Shutdown, S_FINDNAME_SIG_NEW);
		Sig_FuncNotFound(S_FindName);

		gAudEngine.S_PrecacheSound = (sfx_t *(*)(char *))Search_Pattern_From(S_FindName, S_PRECACHESOUND_SIG_NEW);
		Sig_FuncNotFound(S_PrecacheSound);
	
		gAudEngine.SND_Spatialize = (void (*)(aud_channel_t *))Search_Pattern_From(S_PrecacheSound, SND_SPATIALIZE_SIG_NEW);
		Sig_FuncNotFound(SND_Spatialize);

		gAudEngine.S_StartDynamicSound = (void (*)(int, int, sfx_t *, float *, float, float, int, int))Search_Pattern_From(SND_Spatialize, S_STARTDYNAMICSOUND_SIG_NEW);
		Sig_FuncNotFound(S_StartDynamicSound);

		gAudEngine.S_StartStaticSound = (void (*)(int, int, sfx_t *, float *, float, float, int, int))Search_Pattern_From(S_StartDynamicSound, S_STARTSTATICSOUND_SIG_NEW);
		Sig_FuncNotFound(S_StartStaticSound);

		gAudEngine.S_StopSound = (void (*)(int, int))Search_Pattern_From(S_StartStaticSound, S_STOPSOUND_SIG_NEW);
		Sig_FuncNotFound(S_StopSound);

		gAudEngine.S_StopAllSounds = (void (*)(qboolean))Search_Pattern_From(S_StopSound, S_STOPALLSOUNDS_SIG_NEW);
		Sig_FuncNotFound(S_StopAllSounds);

		gAudEngine.S_Update = (void (*)(float *, float *, float *, float *))Search_Pattern_From(S_StopAllSounds, S_UPDATE_SIG_NEW);
		Sig_FuncNotFound(S_Update);

		gAudEngine.S_LoadSound = (aud_sfxcache_t *(*)(sfx_t *, aud_channel_t *))Search_Pattern_From(S_Update, S_LOADSOUND_SIG_NEW);
		Sig_FuncNotFound(S_LoadSound);

		gAudEngine.SequenceGetSentenceByIndex = (sentenceEntry_s*(*)( unsigned int ))Search_Pattern(SEQUENCE_GETSENTENCEBYINDEX_SIG_NEW);
		Sig_FuncNotFound(SequenceGetSentenceByIndex);

		gAudEngine.VoiceSE_Idle = (void (*)(float))Search_Pattern(VOICESE_IDLE_SIG_NEW);
		Sig_FuncNotFound(VoiceSE_Idle);

#ifdef _DEBUG
		gAudEngine.Sys_Error = (void (*)(char *, ...))Search_Pattern(SYS_ERROR_SIG_NEW);
		Sig_FuncNotFound(Sys_Error);
#endif
	}
	else
	{
		gAudEngine.S_Init = (void (*)(void))Search_Pattern(S_INIT_SIG);;
		Sig_FuncNotFound(S_Init);

		gAudEngine.S_Shutdown = (void (*)(void))Search_Pattern_From(S_Init, S_SHUTDOWN_SIG);;
		Sig_FuncNotFound(S_Shutdown);

		gAudEngine.S_FindName = (sfx_t *(*)(char *, int *))Search_Pattern_From(S_Shutdown, S_FINDNAME_SIG);
		Sig_FuncNotFound(S_FindName);

		gAudEngine.S_PrecacheSound = (sfx_t *(*)(char *))Search_Pattern_From(S_FindName, S_PRECACHESOUND_SIG);
		Sig_FuncNotFound(S_PrecacheSound);

		gAudEngine.SND_Spatialize = (void (*)(aud_channel_t *))Search_Pattern_From(S_PrecacheSound, SND_SPATIALIZE_SIG);
		Sig_FuncNotFound(SND_Spatialize);

		gAudEngine.S_StartDynamicSound = (void (*)(int, int, sfx_t *, float *, float, float, int, int))Search_Pattern_From(SND_Spatialize, S_STARTDYNAMICSOUND_SIG);
		Sig_FuncNotFound(S_StartDynamicSound);

		gAudEngine.S_StartStaticSound = (void (*)(int, int, sfx_t *, float *, float, float, int, int))Search_Pattern_From(S_StartDynamicSound, S_STARTSTATICSOUND_SIG);
		Sig_FuncNotFound(S_StartStaticSound);

		gAudEngine.S_StopSound = (void (*)(int, int))Search_Pattern_From(S_StartStaticSound, S_STOPSOUND_SIG);
		Sig_FuncNotFound(S_StopSound);

		gAudEngine.S_StopAllSounds = (void (*)(qboolean))Search_Pattern_From(S_StopSound, S_STOPALLSOUNDS_SIG);
		Sig_FuncNotFound(S_StopAllSounds);

		gAudEngine.S_Update = (void (*)(float *, float *, float *, float *))Search_Pattern_From(S_StopAllSounds, S_UPDATE_SIG);
		Sig_FuncNotFound(S_Update);

		gAudEngine.S_LoadSound = (aud_sfxcache_t *(*)(sfx_t *, aud_channel_t *))Search_Pattern_From(S_Update, S_LOADSOUND_SIG);
		Sig_FuncNotFound(S_LoadSound);

		gAudEngine.SequenceGetSentenceByIndex = (sentenceEntry_s*(*)( unsigned int ))Search_Pattern(SEQUENCE_GETSENTENCEBYINDEX_SIG);
		Sig_FuncNotFound(SequenceGetSentenceByIndex);

		gAudEngine.VoiceSE_Idle = (void (*)(float))Search_Pattern(VOICESE_IDLE_SIG);
		Sig_FuncNotFound(VoiceSE_Idle);

#ifdef _DEBUG
		gAudEngine.Sys_Error = (void (*)(char *, ...))Search_Pattern(SYS_ERROR_SIG);
		Sig_FuncNotFound(Sys_Error);
#endif
	}
	DWORD addr;

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)gAudEngine.SND_Spatialize, 0x10, "\x8B\x0D", Sig_Length("\x8B\x0D"));
	Sig_AddrNotFound(cl_viewentity);
	gAudEngine.cl_viewentity = *(int **)((DWORD)addr + 2);
	gAudEngine.cl_num_entities = gAudEngine.cl_viewentity + 3;
	gAudEngine.cl_parsecount = gAudEngine.cl_viewentity - (0x1789C8 / 4);
	gAudEngine.cl_servercount = gAudEngine.cl_parsecount - 2;
	gAudEngine.cl_waterlevel = gAudEngine.cl_servercount + 0x450 / 4;

	gAudEngine.cl_time = (double *)(gAudEngine.cl_waterlevel + 11);
	gAudEngine.cl_oldtime = gAudEngine.cl_time + 1;

	if(*(byte *)gAudEngine.S_Shutdown != (byte)'\xE8')
		Sig_NotFound(VOX_Shutdown);
	gAudEngine.VOX_Shutdown = (void (*)(void))GetCallAddress((DWORD)gAudEngine.S_Shutdown);

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)gAudEngine.VOX_Shutdown, 0x10, "\xA1", Sig_Length("\xA1"));
	Sig_AddrNotFound(cszrawsentences);
	gAudEngine.cszrawsentences = *(int **)((DWORD)addr + 1);

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)gAudEngine.VOX_Shutdown, 0x20, "\x8B\x04\xB5", Sig_Length("\x8B\x04\xB5"));
	if(!addr)
		addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)gAudEngine.VOX_Shutdown, 0x30, "\xBF\x2A\x2A\x2A\x2A\x8B\x07", Sig_Length("\xBF\x2A\x2A\x2A\x2A\x8B\x07"));
	Sig_AddrNotFound(rgpszrawsentence);
	if(*(byte *)addr == (byte)0x8B)
		gAudEngine.rgpszrawsentence = *(char *(**)[CVOXFILESENTENCEMAX])((DWORD)addr + 3);
	else if(*(byte *)addr == (byte)0xBF)
		gAudEngine.rgpszrawsentence = *(char *(**)[CVOXFILESENTENCEMAX])((DWORD)addr + 1);

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)gAudEngine.S_StopSound, 0x50, "\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04", Sig_Length("\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04"));
	Sig_AddrNotFound(S_FreeChannel);
	gAudEngine.S_FreeChannel = (void (*)(channel_t *))GetCallAddress(addr);

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)gAudEngine.S_FreeChannel, 0x50, "\x50\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04", Sig_Length("\x50\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04"));
	Sig_AddrNotFound(VoiceSE_NotifyFreeChannel);
	gAudEngine.VoiceSE_NotifyFreeChannel = (void (*)(int))GetCallAddress(addr + 1);

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)gAudEngine.VoiceSE_Idle, 0x100, "\xD8\x05\x2A\x2A\x2A\x2A\xD9\x1D", Sig_Length("\xD8\x05\x2A\x2A\x2A\x2A\xD9\x1D"));
	Sig_AddrNotFound(g_SND_VoiceOverdrive);
	gAudEngine.g_SND_VoiceOverdrive = *(float **)(addr + 8);

#define S_STARTUP_SIG "\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x00\x20\x01\x00\xE8"
	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)gAudEngine.S_Init, 0x500, S_STARTUP_SIG, Sig_Length(S_STARTUP_SIG));
	Sig_AddrNotFound(S_Startup);
	gAudEngine.S_Startup = (void (*)(void))GetCallAddress(addr);
}

#ifdef _DEBUG
void Sys_Error(char *fmt, ...)
{
	return gAudEngine.Sys_Error(fmt);
}
#endif

void S_InstallHook(void)
{
	InstallHook(S_Startup);
	InstallHook(S_Init);
	InstallHook(S_Shutdown);	
	InstallHook(S_FindName);
	InstallHook(S_StartDynamicSound);
	InstallHook(S_StartStaticSound);
	InstallHook(S_StopSound);
	InstallHook(S_StopAllSounds);
	InstallHook(S_Update);
	InstallHook(S_LoadSound);
#ifdef _DEBUG
	InstallHook(Sys_Error);
#endif
}