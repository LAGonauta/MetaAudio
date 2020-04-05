#include "Workarounds/NoWorkarounds.hpp"

namespace MetaAudio
{
  float NoWorkarounds::GainWorkaround(float gain)
  {
    return gain;
  }
}