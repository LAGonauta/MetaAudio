#pragma once

#include "../Structures/OcclusionFilter.hpp"
#include "../Structures/Position.hpp"

namespace MetaAudio
{
  class IOcclusionCalculator
  {
  public:
    virtual OcclusionFilter GetParameters(Position listener, Position audioSource, float attenuationMultiplier) = 0;
  };
}