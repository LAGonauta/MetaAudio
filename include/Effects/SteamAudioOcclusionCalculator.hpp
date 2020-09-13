#pragma once
#include <metahook.h>
#include <memory>

#include "IOcclusionCalculator.hpp"
#include "Loaders/SteamAudioMapMeshLoader.hpp"
#include "event_api.h"

namespace MetaAudio
{
  class SteamAudioOcclusionCalculator final : public IOcclusionCalculator
  {
  private:
    static constexpr size_t NUMBER_OCCLUSION_RAYS = 512;
    std::shared_ptr<SteamAudioMapMeshLoader> meshLoader;
    std::shared_ptr<SteamAudio> m_sa;

    const event_api_s& event_api;
    void PlayerTrace(Vector3* start, Vector3* end, pmtrace_s& tr);
  public:
    OcclusionFrequencyGain GetParameters(Vector3 listenerPosition,
      Vector3 listenerAhead,
      Vector3 listenerUp,
      Vector3 audioSourcePosition,
      float sourceRadius,
      float attenuationMultiplier);

    SteamAudioOcclusionCalculator(std::shared_ptr<SteamAudio> sa, std::shared_ptr<SteamAudioMapMeshLoader> meshLoader, const event_api_s& event_api);
  };
}