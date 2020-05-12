#pragma once

namespace MetaAudio
{
  struct FadeResult
  {
    float TotalElapsedTime;
    float Initial;
    float Target;
    float Current;
  };
}