#include <metahook.h>

#include "plugins.h"
#include "snd_local.h"
#include "AudioEngine.hpp"
#include "Loaders/SoundLoader.hpp"

#include <capstone.h>

aud_engine_t gAudEngine = {0};

#define GetCallAddress(addr) (addr + (*(DWORD *)((addr)+1)) + 5)
#define Sig_NotFound(name) Sys_ErrorEx("Could not find entrypoint for %s\nEngine buildnum: %d\n", #name, g_dwEngineBuildnum, g_dwEngineBase, g_dwEngineSize);
#define Sig_VarNotFound(name) if(!name) Sig_NotFound(name)
#define Sig_AddrNotFound(name) if(!addr) Sig_NotFound(name)
#define Sig_FuncNotFound(name) if(!gAudEngine.name) Sig_NotFound(name)

#define Sig_Length(a) (sizeof(a)-1)
#define Search_Pattern(sig) g_pMetaHookAPI->SearchPattern(g_dwEngineTextBase, g_dwEngineTextSize, sig, Sig_Length(sig))
#define Search_Pattern_Data(sig) g_pMetaHookAPI->SearchPattern(g_dwEngineDataBase, g_dwEngineDataSize, sig, Sig_Length(sig))
#define Search_Pattern_Rdata(sig) g_pMetaHookAPI->SearchPattern(g_dwEngineRdataBase, g_dwEngineRdataSize, sig, Sig_Length(sig))
#define Search_Pattern_From_Size(fn, size, sig) g_pMetaHookAPI->SearchPattern((void *)(fn), size, sig, Sig_Length(sig))
#define Search_Pattern_From(fn, sig) g_pMetaHookAPI->SearchPattern((void *)(fn), ((PUCHAR)g_dwEngineTextBase + g_dwEngineTextSize) - (PUCHAR)fn, sig, Sig_Length(sig))
#define Install_InlineHook(fn) g_phook_##fn = g_pMetaHookAPI->InlineHook((void *)gAudEngine.fn, fn, (void **)&gAudEngine.fn)
#define Uninstall_Hook(fn) if(g_phook_##fn){g_pMetaHookAPI->UnHook(g_phook_##fn);g_phook_##fn = NULL;}

//Signatures for the new HL25 engine

#define VOICESE_IDLE_SIG_HL25 "\x55\x8B\xEC\x80\x3D\x2A\x2A\x2A\x2A\x00\xF3\x0F\x10\x2A\x2A\x2A\x2A\x2A\xF3\x0F\x10\x2A\x2A\x2A\x2A\x2A"

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
#define S_LOADSOUND_SIG_NEWEST "\x55\x8B\xEC\x81\xEC\x28\x05\x00\x00\x53\x8B\x5D\x08\x56\x57\x8A"
#define SEQUENCE_GETSENTENCEBYINDEX_SIG_NEW "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x33\xC9\x85\xC0\x2A\x2A\x2A\x8B\x75\x08\x8B\x50\x04"
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
#define SEQUENCE_GETSENTENCEBYINDEX_SIG "\xA1\x2A\x2A\x2A\x2A\x33\xC9\x85\xC0\x2A\x2A\x2A\x2A\x2A\x24\x08\x8B\x50\x04"
#define VOICESE_IDLE_SIG "\xA0\x2A\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\x84\xC0\x74\x2A\xD8\x1D"
#ifdef _DEBUG
#define SYS_ERROR_SIG "\x8B\x4C\x24\x04\x81\xEC\x00\x04\x00\x00\x8D\x84\x24\x08\x04\x00\x00\x8D\x54\x24\x00\x50\x51\x68\x00\x04\x00\x00\x52\xE8\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x83\xC4\x10\x85\xC0"
#endif

void S_FillAddress()
{
  memset(&gAudEngine, 0, sizeof(gAudEngine));

  if (g_dwEngineBuildnum >= 8000)
  {
      if (1)
      {
          /*
.text:01D96050                                     S_Init          proc near               ; CODE XREF: sub_1D65260+32B¡üp
.text:01D96050 68 08 CE E6 01                                      push    offset aSoundInitializ ; "Sound Initialization\n"
.text:01D96055 E8 76 DB F6 FF                                      call    sub_1D03BD0
.text:01D9605A E8 E1 3A 00 00                                      call    sub_1D99B40
.text:01D9605F 68 D8 D9 E5 01                                      push    offset aNosound ; "-nosound"
          */

          const char sigs[] = "Sound Initialization\n";
          auto Sound_Init_String = Search_Pattern_Data(sigs);
          if (!Sound_Init_String)
              Sound_Init_String = Search_Pattern_Rdata(sigs);
          Sig_VarNotFound(Sound_Init_String);

          char pattern[] = "\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A";
          *(DWORD*)(pattern + 1) = (DWORD)Sound_Init_String;
          auto Sound_Init_PushString = (PUCHAR)Search_Pattern(pattern);
          Sig_VarNotFound(Sound_Init_PushString);

          gAudEngine.S_Init = (decltype(gAudEngine.S_Init))Sound_Init_PushString;
          Sig_FuncNotFound(S_Init);
      }
      if (1)
      {
          /*
.text:01D96C7D                                     loc_1D96C7D:                            ; CODE XREF: sub_1D96B60+85¡üj
.text:01D96C7D 68 F4 CF E6 01                                      push    offset aSClearbufferDs_0 ; "S_ClearBuffer: DS::Lock Sound Buffer Fa"...
.text:01D96C82 E8 39 CE F6 FF                                      call    sub_1D03AC0
.text:01D96C87 83 C4 04                                            add     esp, 4
.text:01D96C8A E8 61 F6 FF FF                                      call    S_Stop
.text:01D96C8F 5E                                                  pop     esi
.text:01D96C90 5B                                                  pop     ebx
.text:01D96C91 83 C4 08                                            add     esp, 8
.text:01D96C94 C3                                                  retn
          */

          const char sigs[] = "S_ClearBuffer: DS::Lock";
          auto S_ClearBuffer_String = Search_Pattern_Data(sigs);
          if (!S_ClearBuffer_String)
              S_ClearBuffer_String = Search_Pattern_Rdata(sigs);
          Sig_VarNotFound(S_ClearBuffer_String);

          char pattern[] = "\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\xE8";
          *(DWORD*)(pattern + 1) = (DWORD)S_ClearBuffer_String;
          auto S_ClearBuffer_PushString = (PUCHAR)Search_Pattern(pattern);
          Sig_VarNotFound(S_ClearBuffer_PushString);

          gAudEngine.S_Shutdown = (decltype(gAudEngine.S_Shutdown))GetCallAddress(S_ClearBuffer_PushString + Sig_Length(pattern) - 1);
          Sig_FuncNotFound(S_Shutdown);
      }

      if (1)
      {
          /*
.text:01D97B9A 6A 00                                               push    0
.text:01D97B9C 50                                                  push    eax/edx
.text:01D97B9D E8 5E F4 FF FF                                      call    S_FindName
.text:01D97BA2 83 C4 08                                            add     esp, 8
.text:01D97BA5 85 C0                                               test    eax, eax
.text:01D97BA7 75 26                                               jnz     short loc_1D97BCF
.text:01D97BA9 8D 04 24                                            lea     eax, [esp+104h+var_104]
.text:01D97BAC 50                                                  push    eax
.text:01D97BAD 68 F0 D1 E6 01                                      push    offset aSSayReliableCa_0 ; "S_Say_Reliable: can't find sentence nam"...
.text:01D97BB2 E8 09 BF F6 FF                                      call    sub_1D03AC0
          */

          const char sigs[] = "S_Say_Reliable: can't find sentence";
          auto S_Say_Reliable_String = Search_Pattern_Data(sigs);
          if (!S_Say_Reliable_String)
              S_Say_Reliable_String = Search_Pattern_Rdata(sigs);
          Sig_VarNotFound(S_Say_Reliable_String);

          char pattern[] = "\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08";
          *(DWORD*)(pattern + 1) = (DWORD)S_Say_Reliable_String;
          auto S_Say_Reliable_PushString = (PUCHAR)Search_Pattern(pattern);
          Sig_VarNotFound(S_Say_Reliable_PushString);

          char pattern2[] = "\x6A\x00\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08";
          auto S_FindName_Call = (PUCHAR)Search_Pattern_From_Size((S_Say_Reliable_PushString - 0x60), 0x60, pattern2);
          Sig_VarNotFound(S_FindName_Call);

          gAudEngine.S_FindName = (decltype(gAudEngine.S_FindName))GetCallAddress(S_FindName_Call + 3);
          Sig_FuncNotFound(S_FindName);
      }
      if (1)
      {
          /*
.text:01D210C8 E8 73 AC 06 00                                      call    S_PrecacheSound
.text:01D210CD 68 D8 D2 E3 01                                      push    offset aDebrisFlesh1Wa ; "debris/flesh1.wav"
.text:01D210D2 A3 30 83 D0 02                                      mov     dword_2D08330, eax
.text:01D210D7 E8 64 AC 06 00                                      call    S_PrecacheSound
          */
          const char sigs[] = "debris/flesh1.wav";
          auto S_PrecacheSound_String = Search_Pattern_Data(sigs);
          if (!S_PrecacheSound_String)
              S_PrecacheSound_String = Search_Pattern_Rdata(sigs);
          Sig_VarNotFound(S_PrecacheSound_String);

          char pattern[] = "\xE8\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xA3\x2A\x2A\x2A\x2A\xE8";
          *(DWORD*)(pattern + 6) = (DWORD)S_PrecacheSound_String;
          auto S_PrecacheSound_PushString = (PUCHAR)Search_Pattern(pattern);
          Sig_VarNotFound(S_PrecacheSound_PushString);

          gAudEngine.S_PrecacheSound = (decltype(gAudEngine.S_PrecacheSound))GetCallAddress(S_PrecacheSound_PushString);
          Sig_FuncNotFound(S_PrecacheSound);
      }

      if (1)
      {
#define S_LOADSOUND_SIG_HL25 "\x6A\x00\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08"
          auto S_LoadSound_Call = (PUCHAR)Search_Pattern_From_Size(gAudEngine.S_PrecacheSound, 0x100, S_LOADSOUND_SIG_HL25);
          Sig_VarNotFound(S_LoadSound_Call);

          gAudEngine.S_LoadSound = (decltype(gAudEngine.S_LoadSound))GetCallAddress(S_LoadSound_Call + 3);
          Sig_FuncNotFound(S_LoadSound);
      }

      if (1)
      {
#define SND_SPATIALIZE_SIG_HL25 "\x83\x2A\x00\x74\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x8B\x2A\x2A\x2A\x2A\x2A\x83\xC4\x04"
          auto SND_Spatialize_Call = (PUCHAR)Search_Pattern(SND_SPATIALIZE_SIG_HL25);
          Sig_VarNotFound(SND_Spatialize_Call);

          gAudEngine.SND_Spatialize = (decltype(gAudEngine.SND_Spatialize))GetCallAddress(SND_Spatialize_Call + 6);
          Sig_FuncNotFound(SND_Spatialize);
      }

      if (1)
      {
          /*
.text:01D8C299 68 FC 45 E5 01                                      push    offset aSStartdynamics ; "S_StartDynamicSound: %s volume > 255"
.text:01D8C29E E8 ED 09 FA FF                                      call    sub_1D2CC90
.text:01D8C2A3 83 C4 08                                            add     esp, 8
          */
          const char sigs[] = "Warning: S_StartDynamicSound Ignored";
          auto S_StartDynamicSound_String = Search_Pattern_Data(sigs);
          if (!S_StartDynamicSound_String)
              S_StartDynamicSound_String = Search_Pattern_Rdata(sigs);
          Sig_VarNotFound(S_StartDynamicSound_String);

          char pattern[] = "\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04";
          *(DWORD*)(pattern + 1) = (DWORD)S_StartDynamicSound_String;
          auto S_StartDynamicSound_PushString = (PUCHAR)Search_Pattern(pattern);
          Sig_VarNotFound(S_StartDynamicSound_PushString);

          gAudEngine.S_StartDynamicSound = (decltype(gAudEngine.S_StartDynamicSound))g_pMetaHookAPI->ReverseSearchFunctionBeginEx(S_StartDynamicSound_PushString, 0x300, [](PUCHAR Candidate) {

              if (Candidate[0] == 0x55 &&
                  Candidate[1] == 0x8B &&
                  Candidate[2] == 0xEC)
              {
                  return TRUE;
              }

              if (Candidate[0] == 0x83 &&
                  Candidate[1] == 0xEC &&
                  Candidate[3] == 0xA1)
              {
                  return TRUE;
              }

              return FALSE;
          });

          Sig_FuncNotFound(S_StartDynamicSound);
      }

      if (1)
      {
          /*
.text:01D8C299 68 FC 45 E5 01                                      push    offset aSStartdynamics ; "S_StartDynamicSound: %s volume > 255"
.text:01D8C29E E8 ED 09 FA FF                                      call    sub_1D2CC90
.text:01D8C2A3 83 C4 08                                            add     esp, 8
          */
          const char sigs[] = "Warning: S_StartStaticSound Ignored";
          auto S_StartStaticSound_String = Search_Pattern_Data(sigs);
          if (!S_StartStaticSound_String)
              S_StartStaticSound_String = Search_Pattern_Rdata(sigs);
          Sig_VarNotFound(S_StartStaticSound_String);

          char pattern[] = "\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04";
          *(DWORD*)(pattern + 1) = (DWORD)S_StartStaticSound_String;
          auto S_StartStaticSound_PushString = (PUCHAR)Search_Pattern(pattern);
          Sig_VarNotFound(S_StartStaticSound_PushString);

          gAudEngine.S_StartStaticSound = (decltype(gAudEngine.S_StartStaticSound))g_pMetaHookAPI->ReverseSearchFunctionBeginEx(S_StartStaticSound_PushString, 0x300, [](PUCHAR Candidate) {

              if (Candidate[0] == 0x55 &&
                  Candidate[1] == 0x8B &&
                  Candidate[2] == 0xEC)
              {
                  return TRUE;
              }

              if (Candidate[0] == 0x83 &&
                  Candidate[1] == 0xEC &&
                  Candidate[3] == 0xA1)
              {
                  return TRUE;
              }

              return FALSE;
          });

          Sig_FuncNotFound(S_StartStaticSound);
      }

      if (1)
      {
          /*
.data:01ED73A4 FC 13 E6 01                                         dd offset aSvcStopsound ; "svc_stopsound"
.data:01ED73A8 20 F5 D2 01                                         dd offset CL_StopSound
.data:01ED73AC 11                                                  db  11h
.data:01ED73AD 00                                                  db    0
.data:01ED73AE 00                                                  db    0
.data:01ED73AF 00                                                  db    0
          */
          const char sigs[] = "svc_stopsound";
          auto svc_stopsound_String = Search_Pattern_Data(sigs);
          if (!svc_stopsound_String)
              svc_stopsound_String = Search_Pattern_Rdata(sigs);
          Sig_VarNotFound(svc_stopsound_String);

          char pattern[] = "\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x11\x00\x00\x00";
          *(DWORD*)(pattern + 0) = (DWORD)svc_stopsound_String;
          auto svc_stopsound_Struct = (PUCHAR)Search_Pattern_Data(pattern);
          Sig_VarNotFound(svc_stopsound_Struct);

          PVOID CL_StopSound = *(decltype(CL_StopSound)*)(svc_stopsound_Struct + 4);

          char pattern2[] = "\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08\xC3";
          auto S_StopSound_Call = (PUCHAR)Search_Pattern_From_Size(CL_StopSound, 0x50, pattern2);
          Sig_VarNotFound(S_StopSound_Call);

          gAudEngine.S_StopSound = (decltype(gAudEngine.S_StopSound))GetCallAddress(S_StopSound_Call);
          Sig_FuncNotFound(S_StopSound);
      }

      if (1)
      {
#define S_SOUNDALLSOUNDS_SIG_HL25 "\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\xE8"
          auto S_StopAllSounds_Call = (PUCHAR)Search_Pattern_From_Size(gAudEngine.S_Init, 0x300, S_SOUNDALLSOUNDS_SIG_HL25);
          Sig_VarNotFound(S_StopAllSounds_Call);

          gAudEngine.S_StopAllSounds = (decltype(gAudEngine.S_StopAllSounds))GetCallAddress(S_StopAllSounds_Call);
          Sig_FuncNotFound(S_StopAllSounds);
      }


      if (1)
      {
          /*
.text:101FEBC2                                     loc_101FEBC2:                           ; CODE XREF: S_Update+247¡üj
.text:101FEBC2 53                                                  push    ebx
.text:101FEBC3 68 14 19 2C 10                                      push    offset aI_1     ; "----(%i)----\n"
.text:101FEBC8 E8 B3 D2 FB FF                                      call    sub_101BBE80
          */
          const char sigs[] = "----(%i)----\n";
          auto S_Update_String = Search_Pattern_Data(sigs);
          if (!S_Update_String)
              S_Update_String = Search_Pattern_Rdata(sigs);
          Sig_VarNotFound(S_Update_String);

          char pattern[] = "\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08";
          *(DWORD*)(pattern + 1) = (DWORD)S_Update_String;
          auto S_Update_PushString = (PUCHAR)Search_Pattern(pattern);
          Sig_VarNotFound(S_Update_PushString);

          gAudEngine.S_Update = (decltype(gAudEngine.S_Update))g_pMetaHookAPI->ReverseSearchFunctionBeginEx(S_Update_PushString, 0x500, [](PUCHAR Candidate) {

              if (Candidate[0] == 0x55 &&
                  Candidate[1] == 0x8B &&
                  Candidate[2] == 0xEC)
              {
                  return TRUE;
              }

              if (Candidate[0] == 0x83 &&
                  Candidate[1] == 0x3D &&
                  Candidate[6] == 0x00)
              {
                  return TRUE;
              }

              return FALSE;
          });

          Sig_FuncNotFound(S_Update);
      }

      if (1)
      {
#define SEQUENCE_GETSENTENCEBYINDEX_SIG_HL25 "\x50\xFF\x15\x2A\x2A\x2A\x2A\x50\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08\x85\xC0"
          auto SequenceGetSentenceByIndex_Call = (PUCHAR)Search_Pattern(SEQUENCE_GETSENTENCEBYINDEX_SIG_HL25);
          if (SequenceGetSentenceByIndex_Call)
          {
              Sig_VarNotFound(SequenceGetSentenceByIndex_Call);

              gAudEngine.SequenceGetSentenceByIndex = (decltype(gAudEngine.SequenceGetSentenceByIndex))GetCallAddress(SequenceGetSentenceByIndex_Call + 8);
              Sig_FuncNotFound(SequenceGetSentenceByIndex);
          }
          else
          {
#define SEQUENCE_GETSENTENCEBYINDEX_SIG_GOLDSRC "\x50\xE8\x2A\x2A\x2A\x2A\x50\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08\x85\xC0"
              auto SequenceGetSentenceByIndex_Call = (PUCHAR)Search_Pattern(SEQUENCE_GETSENTENCEBYINDEX_SIG_GOLDSRC);
              Sig_VarNotFound(SequenceGetSentenceByIndex_Call);

              gAudEngine.SequenceGetSentenceByIndex = (decltype(gAudEngine.SequenceGetSentenceByIndex))GetCallAddress(SequenceGetSentenceByIndex_Call + 7);
              Sig_FuncNotFound(SequenceGetSentenceByIndex);
          }
      }

      if (*(byte*)gAudEngine.S_Shutdown != (byte)'\xE8')
          Sig_NotFound(VOX_Shutdown);

      gAudEngine.VOX_Shutdown = (void(*)(void))GetCallAddress((DWORD)gAudEngine.S_Shutdown);

      //TODO use string to locate?
      gAudEngine.VoiceSE_Idle = (decltype(gAudEngine.VoiceSE_Idle))Search_Pattern(VOICESE_IDLE_SIG_HL25);
      if(!gAudEngine.VoiceSE_Idle)
          gAudEngine.VoiceSE_Idle = (decltype(gAudEngine.VoiceSE_Idle))Search_Pattern(VOICESE_IDLE_SIG_NEW);
      Sig_FuncNotFound(VoiceSE_Idle);

      if (1)
      {
          typedef struct
          {
              int unused;
          }SND_Spatializing_Ctx;

          SND_Spatializing_Ctx ctx = { 0 };

          g_pMetaHookAPI->DisasmRanges(gAudEngine.SND_Spatialize, 0x100, [](void* inst, PUCHAR address, size_t instLen, int instCount, int depth, PVOID context)
          {
              auto pinst = (cs_insn*)inst;
              auto ctx = (SND_Spatializing_Ctx*)context;

              if (!gAudEngine.cl_viewentity &&
                  pinst->id == X86_INS_MOV &&
                  pinst->detail->x86.op_count == 2 &&
                  pinst->detail->x86.operands[0].type == X86_OP_REG &&
                  pinst->detail->x86.operands[1].type == X86_OP_MEM &&
                  (PUCHAR)pinst->detail->x86.operands[1].mem.disp > (PUCHAR)g_dwEngineDataBase &&
                  (PUCHAR)pinst->detail->x86.operands[1].mem.disp < (PUCHAR)g_dwEngineDataBase + g_dwEngineDataSize)
              {   //8B 0D CC 37 F0 02                                   mov     ecx, cl_viewentity

                  if (pinst->detail->x86.operands[0].reg == X86_REG_EAX && address[instLen + 0] == 0x33 && address[instLen + 1] == 0xC5)
                  {

                  }
                  else
                  {
                      gAudEngine.cl_viewentity = (decltype(gAudEngine.cl_viewentity))pinst->detail->x86.operands[1].mem.disp;
                  }
              }

              if (!gAudEngine.cl_viewentity &&
                  pinst->id == X86_INS_CMP &&
                  pinst->detail->x86.op_count == 2 &&
                  pinst->detail->x86.operands[0].type == X86_OP_REG &&
                  pinst->detail->x86.operands[1].type == X86_OP_MEM &&
                  (PUCHAR)pinst->detail->x86.operands[1].mem.disp > (PUCHAR)g_dwEngineDataBase &&
                  (PUCHAR)pinst->detail->x86.operands[1].mem.disp < (PUCHAR)g_dwEngineDataBase + g_dwEngineDataSize)
              {//3B 05 8C 61 3E 11                                   cmp     eax, cl_viewentity
                  gAudEngine.cl_viewentity = (decltype(gAudEngine.cl_viewentity))pinst->detail->x86.operands[1].mem.disp;
              }

              if (gAudEngine.cl_viewentity)
                  return TRUE;

              if (address[0] == 0xCC)
                  return TRUE;

              if (pinst->id == X86_INS_RET)
                  return TRUE;

              return FALSE;
          }, 0, &ctx);

          Sig_FuncNotFound(cl_viewentity);
      }

      if (1)
      {
          typedef struct
          {
              int unused;
          }VOX_Shutdown_Ctx;

          VOX_Shutdown_Ctx ctx = { 0 };

          g_pMetaHookAPI->DisasmRanges(gAudEngine.VOX_Shutdown, 0x100, [](void* inst, PUCHAR address, size_t instLen, int instCount, int depth, PVOID context)
          {
              auto pinst = (cs_insn*)inst;
              auto ctx = (VOX_Shutdown_Ctx*)context;

              if (!gAudEngine.cszrawsentences &&
                  pinst->id == X86_INS_MOV &&
                  pinst->detail->x86.op_count == 2 &&
                  pinst->detail->x86.operands[0].type == X86_OP_REG &&
                  pinst->detail->x86.operands[1].type == X86_OP_MEM &&
                  (PUCHAR)pinst->detail->x86.operands[1].mem.disp > (PUCHAR)g_dwEngineDataBase &&
                  (PUCHAR)pinst->detail->x86.operands[1].mem.disp < (PUCHAR)g_dwEngineDataBase + g_dwEngineDataSize)
              {   //mov     eax, cszrawsentences

                  gAudEngine.cszrawsentences = (decltype(gAudEngine.cszrawsentences))pinst->detail->x86.operands[1].mem.disp;
              }

              if (!gAudEngine.cszrawsentences &&
                  pinst->id == X86_INS_CMP &&
                  pinst->detail->x86.op_count == 2 &&
                  pinst->detail->x86.operands[1].type == X86_OP_REG &&
                  pinst->detail->x86.operands[0].type == X86_OP_MEM &&
                  (PUCHAR)pinst->detail->x86.operands[0].mem.disp > (PUCHAR)g_dwEngineDataBase &&
                  (PUCHAR)pinst->detail->x86.operands[0].mem.disp < (PUCHAR)g_dwEngineDataBase + g_dwEngineDataSize)
              {// cmp     cszrawsentences, esi
                  gAudEngine.cszrawsentences = (decltype(gAudEngine.cszrawsentences))pinst->detail->x86.operands[0].mem.disp;
              }

              if (!gAudEngine.rgpszrawsentence &&
                  pinst->id == X86_INS_MOV &&
                  pinst->detail->x86.op_count == 1 &&
                  pinst->detail->x86.operands[0].type == X86_OP_REG &&
                  pinst->detail->x86.operands[1].type == X86_OP_MEM &&
                  (PUCHAR)pinst->detail->x86.operands[1].mem.disp > (PUCHAR)g_dwEngineDataBase &&
                  (PUCHAR)pinst->detail->x86.operands[1].mem.disp < (PUCHAR)g_dwEngineDataBase + g_dwEngineDataSize &&
                  pinst->detail->x86.operands[1].mem.base == 0 &&
                  pinst->detail->x86.operands[1].mem.index != 0 &&
                  pinst->detail->x86.operands[1].mem.scale == 4)
              {//mov     eax, rgpszrawsentence[esi*4]
                  gAudEngine.rgpszrawsentence = (decltype(gAudEngine.rgpszrawsentence))pinst->detail->x86.operands[1].mem.disp;
              }

              if (!gAudEngine.rgpszrawsentence &&
                  pinst->id == X86_INS_PUSH &&
                  pinst->detail->x86.op_count == 1 &&
                  pinst->detail->x86.operands[0].type == X86_OP_MEM &&
                  (PUCHAR)pinst->detail->x86.operands[0].mem.disp > (PUCHAR)g_dwEngineDataBase &&
                  (PUCHAR)pinst->detail->x86.operands[0].mem.disp < (PUCHAR)g_dwEngineDataBase + g_dwEngineDataSize &&
                  pinst->detail->x86.operands[0].mem.base == 0 &&
                  pinst->detail->x86.operands[0].mem.index != 0 &&
                  pinst->detail->x86.operands[0].mem.scale == 4)
              {//push    rgpszrawsentence[esi*4]
                  gAudEngine.rgpszrawsentence = (decltype(gAudEngine.rgpszrawsentence))pinst->detail->x86.operands[0].mem.disp;
              }

              if (gAudEngine.cszrawsentences && gAudEngine.rgpszrawsentence)
                  return TRUE;

              if (address[0] == 0xCC)
                  return TRUE;

              if (pinst->id == X86_INS_RET)
                  return TRUE;

              return FALSE;
          }, 0, &ctx);

          Sig_FuncNotFound(cszrawsentences);
          Sig_FuncNotFound(rgpszrawsentence);
      }

      if (1)
      {
          typedef struct
          {
              PVOID candidate_g_SND_VoiceOverdrive;
              bool ret_found;
          }VoiceSE_Idle_Ctx;

          VoiceSE_Idle_Ctx ctx = { 0 };

          g_pMetaHookAPI->DisasmRanges(gAudEngine.VoiceSE_Idle, 0x1000, [](void* inst, PUCHAR address, size_t instLen, int instCount, int depth, PVOID context)
          {
              auto pinst = (cs_insn*)inst;
              auto ctx = (VoiceSE_Idle_Ctx*)context;

              if ( pinst->id == X86_INS_FSTP && pinst->detail->x86.op_count == 1 &&
                  pinst->detail->x86.operands[0].type == X86_OP_MEM &&
                  pinst->detail->x86.operands[0].mem.base == 0)
              {   //8B 0D CC 37 F0 02                                   mov     ecx, cl_viewentity

                 ctx->candidate_g_SND_VoiceOverdrive = (PVOID)pinst->detail->x86.operands[0].mem.disp;
              }

              if (pinst->id == X86_INS_MOVSS && pinst->detail->x86.op_count == 2 &&
                  pinst->detail->x86.operands[0].type == X86_OP_MEM &&
                  (PUCHAR)pinst->detail->x86.operands[0].mem.disp > (PUCHAR)g_dwEngineDataBase &&
                  (PUCHAR)pinst->detail->x86.operands[0].mem.disp < (PUCHAR)g_dwEngineDataBase + g_dwEngineDataSize)
              {   //8B 0D CC 37 F0 02                                   mov     ecx, cl_viewentity

                  ctx->candidate_g_SND_VoiceOverdrive = (PVOID)pinst->detail->x86.operands[0].mem.disp;
              }

              if (address[0] == 0xCC)
                  return TRUE;

              if (pinst->id == X86_INS_RET)
              {
                  ctx->ret_found = true;
                  return TRUE;
              }

              return FALSE;
          }, 0, &ctx);

          if (ctx.ret_found && ctx.candidate_g_SND_VoiceOverdrive)
          {
              gAudEngine.g_SND_VoiceOverdrive = (decltype(gAudEngine.g_SND_VoiceOverdrive))ctx.candidate_g_SND_VoiceOverdrive;
          }

          Sig_FuncNotFound(g_SND_VoiceOverdrive);
      }


      if (1)
      {
          /*
.text:01D8E3AA                                     loc_1D8E3AA:                            ; CODE XREF: sub_1D8E280+6E¡üj
.text:01D8E3AA 68 D0 52 E5 01                                      push    offset aSTransferstere ; "S_TransferStereo16: DS::Lock Sound Buff"...
.text:01D8E3AF
.text:01D8E3AF                                     loc_1D8E3AF:                            ; CODE XREF: sub_1D8E280+14D¡ýj
.text:01D8E3AF E8 BC E7 F9 FF                                      call    sub_1D2CB70
.text:01D8E3B4 83 C4 04                                            add     esp, 4
.text:01D8E3B7 E8 E4 D6 FF FF                                      call    S_Shutdown
.text:01D8E3BC E8 8F D3 FF FF                                      call    S_Startup
          */

          const char sigs[] = "S_TransferStereo16: DS::Lock Sound Buff";
          auto S_TransferStereo16_String = Search_Pattern_Data(sigs);
          if (!S_TransferStereo16_String)
              S_TransferStereo16_String = Search_Pattern_Rdata(sigs);
          Sig_VarNotFound(S_TransferStereo16_String);

          char pattern[] = "\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\xE8\x2A\x2A\x2A\x2A\xE8";
          *(DWORD*)(pattern + 1) = (DWORD)S_TransferStereo16_String;
          auto S_TransferStereo16_PushString = (PUCHAR)Search_Pattern(pattern);
          Sig_VarNotFound(S_TransferStereo16_PushString);

          gAudEngine.S_Startup = (decltype(gAudEngine.S_Startup))GetCallAddress(S_TransferStereo16_PushString + Sig_Length(pattern) - 1);
          Sig_FuncNotFound(S_Startup);
      }
  }
  else if (g_dwEngineBuildnum >= 5953)
  {
    gAudEngine.S_Init = (void(*)(void))Search_Pattern(S_INIT_SIG_NEW);
    Sig_FuncNotFound(S_Init);

    gAudEngine.S_Shutdown = (void(*)(void))Search_Pattern_From(gAudEngine.S_Init, S_SHUTDOWN_SIG_NEW);
    Sig_FuncNotFound(S_Shutdown);

    gAudEngine.S_FindName = (sfx_t *(*)(char *, int *))Search_Pattern_From(gAudEngine.S_Shutdown, S_FINDNAME_SIG_NEW);
    Sig_FuncNotFound(S_FindName);

    gAudEngine.S_PrecacheSound = (sfx_t *(*)(char *))Search_Pattern_From(gAudEngine.S_FindName, S_PRECACHESOUND_SIG_NEW);
    Sig_FuncNotFound(S_PrecacheSound);

    gAudEngine.SND_Spatialize = (void(*)(aud_channel_t *))Search_Pattern_From(gAudEngine.S_PrecacheSound, SND_SPATIALIZE_SIG_NEW);
    Sig_FuncNotFound(SND_Spatialize);

    gAudEngine.S_StartDynamicSound = (void(*)(int, int, sfx_t *, float *, float, float, int, int))Search_Pattern_From(gAudEngine.SND_Spatialize, S_STARTDYNAMICSOUND_SIG_NEW);
    Sig_FuncNotFound(S_StartDynamicSound);

    gAudEngine.S_StartStaticSound = (void(*)(int, int, sfx_t *, float *, float, float, int, int))Search_Pattern_From(gAudEngine.S_StartDynamicSound, S_STARTSTATICSOUND_SIG_NEW);
    Sig_FuncNotFound(S_StartStaticSound);

    gAudEngine.S_StopSound = (void(*)(int, int))Search_Pattern_From(gAudEngine.S_StartStaticSound, S_STOPSOUND_SIG_NEW);
    Sig_FuncNotFound(S_StopSound);

    gAudEngine.S_StopAllSounds = (void(*)(qboolean))Search_Pattern_From(gAudEngine.S_StopSound, S_STOPALLSOUNDS_SIG_NEW);
    Sig_FuncNotFound(S_StopAllSounds);

    gAudEngine.S_Update = (void(*)(float *, float *, float *, float *))Search_Pattern_From(gAudEngine.S_StopAllSounds, S_UPDATE_SIG_NEW);
    Sig_FuncNotFound(S_Update);

    if (g_dwEngineBuildnum >= 8279)
    {
      gAudEngine.S_LoadSound = (aud_sfxcache_t *(*)(sfx_t *, aud_channel_t *))Search_Pattern_From(gAudEngine.S_Update, S_LOADSOUND_SIG_NEWEST);
    }
    else
    {
      gAudEngine.S_LoadSound = (aud_sfxcache_t *(*)(sfx_t *, aud_channel_t *))Search_Pattern_From(gAudEngine.S_Update, S_LOADSOUND_SIG_NEW);
    }
    Sig_FuncNotFound(S_LoadSound);

    gAudEngine.SequenceGetSentenceByIndex = (sentenceEntry_s*(*)(unsigned int))Search_Pattern(SEQUENCE_GETSENTENCEBYINDEX_SIG_NEW);
    Sig_FuncNotFound(SequenceGetSentenceByIndex);

    gAudEngine.VoiceSE_Idle = (void(*)(float))Search_Pattern(VOICESE_IDLE_SIG_NEW);
    Sig_FuncNotFound(VoiceSE_Idle);

    if (*(byte*)gAudEngine.S_Shutdown != (byte)'\xE8')
        Sig_NotFound(VOX_Shutdown);

    gAudEngine.VOX_Shutdown = (void(*)(void))GetCallAddress((DWORD)gAudEngine.S_Shutdown);


  }
  else
  {
    gAudEngine.S_Init = (void(*)(void))Search_Pattern(S_INIT_SIG);;
    Sig_FuncNotFound(S_Init);

    gAudEngine.S_Shutdown = (void(*)(void))Search_Pattern_From(gAudEngine.S_Init, S_SHUTDOWN_SIG);
    Sig_FuncNotFound(S_Shutdown);

    gAudEngine.S_FindName = (sfx_t *(*)(char *, int *))Search_Pattern_From(gAudEngine.S_Shutdown, S_FINDNAME_SIG);
    Sig_FuncNotFound(S_FindName);

    gAudEngine.S_PrecacheSound = (sfx_t *(*)(char *))Search_Pattern_From(gAudEngine.S_FindName, S_PRECACHESOUND_SIG);
    Sig_FuncNotFound(S_PrecacheSound);

    gAudEngine.SND_Spatialize = (void(*)(aud_channel_t *))Search_Pattern_From(gAudEngine.S_PrecacheSound, SND_SPATIALIZE_SIG);
    Sig_FuncNotFound(SND_Spatialize);

    gAudEngine.S_StartDynamicSound = (void(*)(int, int, sfx_t *, float *, float, float, int, int))Search_Pattern_From(gAudEngine.SND_Spatialize, S_STARTDYNAMICSOUND_SIG);
    Sig_FuncNotFound(S_StartDynamicSound);

    gAudEngine.S_StartStaticSound = (void(*)(int, int, sfx_t *, float *, float, float, int, int))Search_Pattern_From(gAudEngine.S_StartDynamicSound, S_STARTSTATICSOUND_SIG);
    Sig_FuncNotFound(S_StartStaticSound);

    gAudEngine.S_StopSound = (void(*)(int, int))Search_Pattern_From(gAudEngine.S_StartStaticSound, S_STOPSOUND_SIG);
    Sig_FuncNotFound(S_StopSound);

    gAudEngine.S_StopAllSounds = (void(*)(qboolean))Search_Pattern_From(gAudEngine.S_StopSound, S_STOPALLSOUNDS_SIG);
    Sig_FuncNotFound(S_StopAllSounds);

    gAudEngine.S_Update = (void(*)(float *, float *, float *, float *))Search_Pattern_From(gAudEngine.S_StopAllSounds, S_UPDATE_SIG);
    Sig_FuncNotFound(S_Update);

    gAudEngine.S_LoadSound = (aud_sfxcache_t *(*)(sfx_t *, aud_channel_t *))Search_Pattern_From(gAudEngine.S_Update, S_LOADSOUND_SIG);
    Sig_FuncNotFound(S_LoadSound);

    gAudEngine.SequenceGetSentenceByIndex = (sentenceEntry_s*(*)(unsigned int))Search_Pattern(SEQUENCE_GETSENTENCEBYINDEX_SIG);
    Sig_FuncNotFound(SequenceGetSentenceByIndex);

    gAudEngine.VoiceSE_Idle = (void(*)(float))Search_Pattern(VOICESE_IDLE_SIG);
    Sig_FuncNotFound(VoiceSE_Idle);


    if (*(byte*)gAudEngine.S_Shutdown != (byte)'\xE8')
        Sig_NotFound(VOX_Shutdown);

    gAudEngine.VOX_Shutdown = (void(*)(void))GetCallAddress((DWORD)gAudEngine.S_Shutdown);

  }

  ULONG_PTR addr;

  if (!gAudEngine.cl_viewentity)
  {
      addr = (ULONG_PTR)Search_Pattern_From_Size((void*)gAudEngine.SND_Spatialize, 0x10, "\x8B\x0D");

      Sig_AddrNotFound(cl_viewentity);

      gAudEngine.cl_viewentity = *(int**)((ULONG_PTR)addr + 2);
  }

  gAudEngine.cl_num_entities = gAudEngine.cl_viewentity + 3;
  gAudEngine.cl_parsecount = gAudEngine.cl_viewentity - (0x1789C8 / 4);
  gAudEngine.cl_servercount = gAudEngine.cl_parsecount - 2;
  gAudEngine.cl_waterlevel = gAudEngine.cl_servercount + 0x450 / 4;

  gAudEngine.cl_time = (double *)(gAudEngine.cl_waterlevel + 11);
  gAudEngine.cl_oldtime = gAudEngine.cl_time + 1;

  if (!gAudEngine.cszrawsentences)
  {
      auto addr = (DWORD)Search_Pattern_From_Size((void*)gAudEngine.VOX_Shutdown, 0x10, "\xA1");
      Sig_AddrNotFound(cszrawsentences);
      gAudEngine.cszrawsentences = *(int**)((DWORD)addr + 1);
  }

  if (!gAudEngine.rgpszrawsentence)
  {
      addr = (DWORD)Search_Pattern_From_Size((void*)gAudEngine.VOX_Shutdown, 0x20, "\x8B\x04\xB5");
      if (!addr)
          addr = (DWORD)Search_Pattern_From_Size((void*)gAudEngine.VOX_Shutdown, 0x30, "\xBF\x2A\x2A\x2A\x2A\x8B\x07");

      Sig_AddrNotFound(rgpszrawsentence);

      if (*(byte*)addr == (byte)0x8B)
          gAudEngine.rgpszrawsentence = *(char* (**)[CVOXFILESENTENCEMAX])((DWORD)addr + 3);
      else if (*(byte*)addr == (byte)0xBF)
          gAudEngine.rgpszrawsentence = *(char* (**)[CVOXFILESENTENCEMAX])((DWORD)addr + 1);
  }

  if (!gAudEngine.S_FreeChannel)
  {
      addr = (DWORD)Search_Pattern_From_Size((void*)gAudEngine.S_StopSound, 0x50, "\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04");
      if (!addr)
          addr = (DWORD)Search_Pattern_From_Size((void*)gAudEngine.S_StopSound, 0x50, "\xE8\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x83\xC4\x04");
      Sig_AddrNotFound(S_FreeChannel);
      gAudEngine.S_FreeChannel = (void(*)(channel_t*))GetCallAddress(addr);
  }

  if (!gAudEngine.VoiceSE_NotifyFreeChannel)
  {
      addr = (DWORD)Search_Pattern_From_Size((void*)gAudEngine.S_FreeChannel, 0x50, "\x50\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04");
      Sig_AddrNotFound(VoiceSE_NotifyFreeChannel);
      gAudEngine.VoiceSE_NotifyFreeChannel = (void(*)(int))GetCallAddress(addr + 1);
  }

  if (!gAudEngine.g_SND_VoiceOverdrive)
  {
      addr = (DWORD)Search_Pattern_From_Size((void*)gAudEngine.VoiceSE_Idle, 0x100, "\xD8\x05\x2A\x2A\x2A\x2A\xD9\x1D");
      Sig_AddrNotFound(g_SND_VoiceOverdrive);
      gAudEngine.g_SND_VoiceOverdrive = *(float**)(addr + 8);
  }

  if (!gAudEngine.S_Startup)
  {
#define S_STARTUP_SIG "\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x00\x20\x01\x00\xE8"
      addr = (DWORD)Search_Pattern_From_Size((void*)gAudEngine.S_Init, 0x500, S_STARTUP_SIG);
      Sig_AddrNotFound(S_Startup);
      gAudEngine.S_Startup = (void(*)(void))GetCallAddress(addr);
  }

  if (!gAudEngine.SNDDMA_Init)
  {
#define SNDDMA_INIT_SIG "\xE8\x2A\x2A\x2A\x2A\x85\xC0\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8"
      addr = (DWORD)Search_Pattern_From_Size((void*)gAudEngine.S_Startup, 0x80, SNDDMA_INIT_SIG);
      Sig_AddrNotFound(SNDDMA_Init);
      gAudEngine.SNDDMA_Init = (int(*)(void))GetCallAddress(addr);
  }
}

static MetaAudio::AudioEngine* p_engine = nullptr;
static MetaAudio::SoundLoader* p_loader = nullptr;

//S_Startup has been inlined into S_Init in the new HL25 engine. use SNDDMA_Init instead
//static void S_Startup() { p_engine->S_Startup(); }
static int SNDDMA_Init() { return p_engine->SNDDMA_Init(); }
static void S_Init() { p_engine->S_Init(); }
static void S_Shutdown() { p_engine->S_Shutdown(); }
static sfx_t* S_FindName(char* name, int* pfInCache) { return p_engine->S_FindName(name, pfInCache); }
static void S_StartDynamicSound(int entnum, int entchannel, sfx_t* sfx, float* origin, float fvol, float attenuation, int flags, int pitch) { p_engine->S_StartDynamicSound(entnum, entchannel, sfx, origin, fvol, attenuation, flags, pitch); };
static void S_StartStaticSound(int entnum, int entchannel, sfx_t* sfx, float* origin, float fvol, float attenuation, int flags, int pitch) { p_engine->S_StartStaticSound(entnum, entchannel, sfx, origin, fvol, attenuation, flags, pitch); };
static void S_StopSound(int entnum, int entchannel) { p_engine->S_StopSound(entnum, entchannel); };
static void S_StopAllSounds(qboolean clear) { p_engine->S_StopAllSounds(clear); }
static void S_Update(float* origin, float* forward, float* right, float* up) { p_engine->S_Update(origin, forward, right, up); }
static aud_sfxcache_t* S_LoadSound(sfx_t* s, aud_channel_t* ch) { return p_loader->S_LoadSound(s, ch); }

static hook_t* g_phook_SNDDMA_Init = NULL;
static hook_t* g_phook_S_Init = NULL;
static hook_t* g_phook_S_Shutdown = NULL;
static hook_t* g_phook_S_FindName = NULL;
static hook_t* g_phook_S_StartDynamicSound = NULL;
static hook_t* g_phook_S_StartStaticSound = NULL;
static hook_t* g_phook_S_StopSound = NULL;
static hook_t* g_phook_S_StopAllSounds = NULL;
static hook_t* g_phook_S_Update = NULL;
static hook_t* g_phook_S_LoadSound = NULL;

void S_InstallHook(MetaAudio::AudioEngine* engine, MetaAudio::SoundLoader* loader)
{
  p_engine = engine;
  p_loader = loader;
  Install_InlineHook(SNDDMA_Init);
  Install_InlineHook(S_Init);
  Install_InlineHook(S_Shutdown);
  Install_InlineHook(S_FindName);
  Install_InlineHook(S_StartDynamicSound);
  Install_InlineHook(S_StartStaticSound);
  Install_InlineHook(S_StopSound);
  Install_InlineHook(S_StopAllSounds);
  Install_InlineHook(S_Update);
  Install_InlineHook(S_LoadSound);
}

void S_UninstallHook()
{
    Uninstall_Hook(SNDDMA_Init);
    Uninstall_Hook(S_Init);
    Uninstall_Hook(S_Shutdown);
    Uninstall_Hook(S_FindName);
    Uninstall_Hook(S_StartDynamicSound);
    Uninstall_Hook(S_StartStaticSound);
    Uninstall_Hook(S_StopSound);
    Uninstall_Hook(S_StopAllSounds);
    Uninstall_Hook(S_Update);
    Uninstall_Hook(S_LoadSound);
}