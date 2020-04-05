#pragma once

#include "IOcclusionCalculator.hpp"

namespace MetaAudio
{
  class SteamAudioOcclusionCalculator final : public IOcclusionCalculator
  {
  public:
    OcclusionFilter GetParameters(Position listener, Position audioSource, float attenuationMultiplier);
  };
}