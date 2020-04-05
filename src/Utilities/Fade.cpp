#include "Utilities/Fade.hpp"

namespace MetaAudio
{
  float Fade::Lerp(float inital_value, float final_value, float fraction)
  {
    return (inital_value * (1.0f - fraction)) + (final_value * fraction);
  };

  float Fade::ToNewValue(bool force_final,
                          float& elapsed_time,
                          float& initial_value,
                          float current_value,
                          float& last_final_value,
                          float final_value,
                          float frametime)
  {
    if (last_final_value != final_value)
    {
      elapsed_time = 0;
      initial_value = current_value;
      last_final_value = final_value;
    }

    if (frametime == 0.0f)
    {
      return current_value;
    }
    elapsed_time += frametime;

    if (elapsed_time >= AL_SND_GAIN_FADE_TIME)
    {
      elapsed_time = AL_SND_GAIN_FADE_TIME;
      return final_value;
    }

    return Lerp(initial_value, final_value, elapsed_time / AL_SND_GAIN_FADE_TIME);
  }
}