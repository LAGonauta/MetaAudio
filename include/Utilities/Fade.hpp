#pragma once

#include "Structures/FadeResult.hpp"

namespace MetaAudio
{
  class Fade
  {
  private:
    static constexpr float AL_SND_GAIN_FADE_TIME = 0.25f;

    float Lerp(float inital_value, float final_value, float fraction);

  public:
    FadeResult ToNewValue(FadeResult input, float final_value, float frametime);
  };
}