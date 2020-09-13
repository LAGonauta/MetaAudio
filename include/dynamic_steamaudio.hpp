#pragma once

#include <memory>

#include "phonon.h"
#include "wtypes.h"

namespace MetaAudio
{
  class SteamAudio
  {
  private:
    bool loaded = false;
    std::shared_ptr<HINSTANCE> library_instance = nullptr;
  public:
    decltype(&iplCleanup) iplCleanup = nullptr;
    decltype(&iplCreateContext) iplCreateContext = nullptr;
    decltype(&iplCreateEnvironment) iplCreateEnvironment = nullptr;
    decltype(&iplCreateScene) iplCreateScene = nullptr;
    decltype(&iplCreateStaticMesh) iplCreateStaticMesh = nullptr;
    decltype(&iplDestroyContext) iplDestroyContext = nullptr;
    decltype(&iplDestroyEnvironment) iplDestroyEnvironment = nullptr;
    decltype(&iplDestroyScene) iplDestroyScene = nullptr;
    decltype(&iplDestroyStaticMesh) iplDestroyStaticMesh = nullptr;
    decltype(&iplGetDirectSoundPath) iplGetDirectSoundPath = nullptr;
    decltype(&iplGetMixedEnvironmentalAudio) iplGetMixedEnvironmentalAudio = nullptr;

    SteamAudio();
  };
}