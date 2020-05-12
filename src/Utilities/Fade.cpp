#include "Utilities/Fade.hpp"

namespace MetaAudio
{
  float Fade::Lerp(float inital_value, float final_value, float fraction)
  {
    return (inital_value * (1.0f - fraction)) + (final_value * fraction);
  };

  FadeResult Fade::ToNewValue(FadeResult input,
                          float current_target,
                          float frametime)
  {
    if (input.Target != current_target)
    {
      input.TotalElapsedTime = 0;
      input.Initial = input.Current;
      input.Target = current_target;
    }

    if (frametime == 0.0f)
    {
      return input;
    }

    input.TotalElapsedTime += frametime;

    if (input.TotalElapsedTime >= AL_SND_GAIN_FADE_TIME)
    {
      input.TotalElapsedTime = AL_SND_GAIN_FADE_TIME;
      input.Current = current_target;
      return input;
    }

    input.Current = Lerp(input.Initial, current_target, input.TotalElapsedTime / AL_SND_GAIN_FADE_TIME);
    return input;
  }
}