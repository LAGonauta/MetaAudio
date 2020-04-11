#pragma once
#include <memory>

#include "IOcclusionCalculator.hpp"
#include "Loaders/SteamAudioMapMeshLoader.hpp"

namespace MetaAudio
{
  class SteamAudioOcclusionCalculator final : public IOcclusionCalculator
  {
  private:
    std::shared_ptr<SteamAudioMapMeshLoader> meshLoader;

  public:
    OcclusionFilter GetParameters(Vector3 listenerPosition,
      Vector3 listenerAhead,
      Vector3 listenerUp,
      Vector3 audioSourcePosition,
      float sourceRadius,
      float attenuationMultiplier);

    SteamAudioOcclusionCalculator(std::shared_ptr<SteamAudioMapMeshLoader> meshLoader);
  };
}