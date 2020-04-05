#pragma once

namespace MetaAudio
{
  class Fade
  {
  private:
    static constexpr float AL_SND_GAIN_FADE_TIME = 0.25f;

    float Lerp(float inital_value, float final_value, float fraction);

  public:
    float ToNewValue(bool force_final,
                      float& elapsed_time,
                      float& initial_value,
                      float current_value,
                      float& last_final_value,
                      float final_value,
                      float frametime);
  };
}