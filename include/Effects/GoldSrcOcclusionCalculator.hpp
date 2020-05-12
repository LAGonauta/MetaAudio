#pragma once

#include "IOcclusionCalculator.hpp"
#include "Utilities/VectorUtils.hpp"
#include "event_api.h"

namespace MetaAudio
{
  class GoldSrcOcclusionCalculator final : public IOcclusionCalculator
  {
  private:
    // Attenuate -2.7dB per meter? Probably should be more.
    // Attenuation per wall inch in dB
    static constexpr float TRANSMISSION_ATTN_PER_INCH = -2.7f * AL_UnitToMeters;

    const event_api_s& event_api;
    void PlayerTrace(Vector3* start, Vector3* end, pmtrace_s& tr);
  public:
    GoldSrcOcclusionCalculator(const event_api_s& event_api);

    OcclusionFrequencyGain GetParameters(Vector3 listenerPosition,
      Vector3 listenerAhead,
      Vector3 listenerUp,
      Vector3 audioSourcePosition,
      float sourceRadius,
      float attenuationMultiplier);
  };
}