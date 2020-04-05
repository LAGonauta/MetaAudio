#pragma once

namespace MetaAudio
{
  class IWorkarounds
  {
  public:
    virtual float GainWorkaround(float gain) = 0;
  };
}