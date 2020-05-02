#pragma once

#include "phonon.h"

namespace MetaAudio
{
  struct SteamAudio
  {
    decltype(&iplDestroyEnvironment) iplDestroyEnvironment;
    decltype(&iplDestroyScene) iplDestroyScene;
    decltype(&iplDestroyStaticMesh) iplDestroyStaticMesh;
    decltype(&iplGetDirectSoundPath) iplGetDirectSoundPath;
    decltype(&iplCreateScene) iplCreateScene;
    decltype(&iplCreateStaticMesh) iplCreateStaticMesh;
    decltype(&iplCreateEnvironment) iplCreateEnvironment;
    decltype(&iplCreateContext) iplCreateContext;
    decltype(&iplDestroyContext) iplDestroyContext;
    decltype(&iplCleanup) iplCleanup;
  };

#define SetSteamAudioFunctionPointer(target, library, __function_name__) \
    target.__function_name__ = reinterpret_cast<decltype(MetaAudio::SteamAudio::__function_name__)>(GetProcAddress(library, #__function_name__));
}

extern MetaAudio::SteamAudio gSteamAudio;