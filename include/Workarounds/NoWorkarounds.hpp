#pragma once

#include "IWorkarounds.hpp"

namespace MetaAudio
{
  class NoWorkarounds final : public IWorkarounds
  {
  public:
    float GainWorkaround(float gain) override;
  };
}