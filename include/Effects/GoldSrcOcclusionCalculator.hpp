#pragma once

#include <limits>

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

    const event_api_s* event_api;
    void PlayerTrace(Position start, Position end, pmtrace_s& tr);
  public:
    GoldSrcOcclusionCalculator(event_api_s* event_api);

    OcclusionFilter GetParameters(Position listener, Position audioSource, float attenuationMultiplier);
  };
}