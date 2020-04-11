#pragma once

#include "../Structures/OcclusionFilter.hpp"
#include "../Structures/Vector3.hpp"

namespace MetaAudio
{
  class IOcclusionCalculator
  {
  public:
    // Vectors should be in meters and using OpenAL coordinate orientation
    virtual OcclusionFilter GetParameters(Vector3 listenerPosition,
      Vector3 listenerAhead,
      Vector3 listenerUp,
      Vector3 audioSourcePosition,
      float sourceRadius,
      float attenuationMultiplier) = 0;
  };
}