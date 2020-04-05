#pragma once

#include <limits>

#include "IOcclusionCalculator.hpp"
#include "snd_utilities.hpp"
#include "event_api.h"

namespace MetaAudio
{
  class GoldSrcOcclusionCalculator : public IOcclusionCalculator
  {
  private:
    // Attenuate -2.7dB per meter? Probably should be more.
    // Attenuation per wall inch in dB
    static constexpr float TRANSMISSION_ATTN_PER_INCH = -2.7f * AL_UnitToMeters;

    // Creative X-Fi's are buggy with the direct filter gain set to 1.0f,
    // they get stuck.
    static constexpr float gain_epsilon = 1.0f - std::numeric_limits<float>::epsilon();

    const event_api_s* event_api;
    void PlayerTrace(Position start, Position end, pmtrace_s& tr);
  public:
    GoldSrcOcclusionCalculator(event_api_s* event_api);

    OcclusionFilter GetParameters(Position listener, Position audioSource, float attenuationMultiplier);
  };
}