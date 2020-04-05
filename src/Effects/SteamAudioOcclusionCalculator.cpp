#include "Effects/SteamAudioOcclusionCalculator.hpp"

namespace MetaAudio
{
  OcclusionFilter SteamAudioOcclusionCalculator::GetParameters(Position listener, Position audioSource, float attenuationMultiplier)
  {
    return OcclusionFilter{ 1.0f, 1.0f, 1.0f };
  }
}