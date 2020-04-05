#pragma once

#include <limits>

#include "IWorkarounds.hpp"

namespace MetaAudio
{
  class XFiWorkarounds final : public IWorkarounds
  {
  private:
    // Creative X-Fi's are buggy with the direct filter gain set to 1.0f,
    // they get stuck. One must convert 1.0f to gain_epsilon when setting them.
    static constexpr float gain_epsilon = 1.0f - std::numeric_limits<float>::epsilon();
  public:
    float GainWorkaround(float gain) override;
  };
}