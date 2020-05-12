#pragma once

#include "../Structures/OcclusionFrequencyGain.hpp"
#include "../Structures/Vector3.hpp"

namespace MetaAudio
{
  class IOcclusionCalculator
  {
  public:
    // Vectors should be in meters and using OpenAL coordinate orientation
    virtual OcclusionFrequencyGain GetParameters(Vector3 listenerPosition,
      Vector3 listenerAhead,
      Vector3 listenerUp,
      Vector3 audioSourcePosition,
      float sourceRadius,
      float attenuationMultiplier) = 0;
  };
}