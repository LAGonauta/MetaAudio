#include "Effects/SteamAudioOcclusionCalculator.hpp"

namespace MetaAudio
{
  SteamAudioOcclusionCalculator::SteamAudioOcclusionCalculator(std::shared_ptr<SteamAudioMapMeshLoader> meshLoader) : meshLoader(meshLoader)
  {
  }

  static IPLVector3 ToIPLVector3(const Vector3& vector)
  {
    return { vector.X, vector.Y, vector.Z };
  }

  OcclusionFilter SteamAudioOcclusionCalculator::GetParameters(
    Vector3 listenerPosition,
    Vector3 listenerAhead,
    Vector3 listenerUp,
    Vector3 audioSourcePosition,
    float sourceRadius,
    float attenuationMultiplier
    )
  {
    if (attenuationMultiplier == 0.0f)
    {
      return OcclusionFilter{ 1.0f, 1.0f, 1.0f };
    }

    IPLSource source{ToIPLVector3(audioSourcePosition)};
    auto env = meshLoader->get_current_environment();
    IPLDirectSoundPath result{};
    result = iplGetDirectSoundPath(
      env,
      ToIPLVector3(listenerPosition),
      ToIPLVector3(listenerAhead),
      ToIPLVector3(listenerUp),
      source,
      sourceRadius,
      IPLDirectOcclusionMode::IPL_DIRECTOCCLUSION_TRANSMISSIONBYFREQUENCY,
      IPLDirectOcclusionMethod::IPL_DIRECTOCCLUSION_VOLUMETRIC
      );

    auto getFactor = [=](const IPLDirectSoundPath& path, size_t index) { return std::clamp((path.occlusionFactor + (1 - path.occlusionFactor) * path.transmissionFactor[index]) * attenuationMultiplier, 0.0f, 1.0f); };
    auto ret = OcclusionFilter{ getFactor(result, 0), getFactor(result, 1), getFactor(result, 2) };
    return ret;
  }
}