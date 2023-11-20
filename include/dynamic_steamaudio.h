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
    decltype(&iplSceneCreate) iplSceneCreate;
    decltype(&iplStaticMeshCreate) iplStaticMeshCreate;
    decltype(&iplEnvironmentCreate) iplEnvironmentCreate;
    decltype(&iplContextCreate) iplContextCreate;
    decltype(&iplContextRelease) iplContextRelease;
    decltype(&iplCleanup) iplCleanup;
  };

#define SetSteamAudioFunctionPointer(target, library, __function_name__) \
    target.__function_name__ = reinterpret_cast<decltype(MetaAudio::SteamAudio::__function_name__)>(GetProcAddress(library, #__function_name__));
}

extern MetaAudio::SteamAudio gSteamAudio;