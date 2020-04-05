#include "Workarounds/XFiWorkarounds.hpp"

namespace MetaAudio
{
  float XFiWorkarounds::GainWorkaround(float gain)
  {
    return gain == 1.0f ? gain_epsilon : gain;
  }
}