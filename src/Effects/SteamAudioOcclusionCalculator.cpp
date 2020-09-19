#include "Effects/SteamAudioOcclusionCalculator.hpp"
#include "pm_defs.h"
#include "Utilities/VectorUtils.hpp"

namespace MetaAudio
{
  SteamAudioOcclusionCalculator::SteamAudioOcclusionCalculator(std::shared_ptr<SteamAudio> sa, std::shared_ptr<SteamAudioMapMeshLoader> meshLoader, const event_api_s& event_api)
    : meshLoader(meshLoader), event_api(event_api), m_sa(sa)
  {
  }

  void SteamAudioOcclusionCalculator::PlayerTrace(Vector3* start, Vector3* end, pmtrace_s& tr)
  {
    // 0 = regular player hull, 1 = ducked player hull, 2 = point hull
    event_api.EV_SetTraceHull(2);
    event_api.EV_PlayerTrace(reinterpret_cast<float*>(start), reinterpret_cast<float*>(end), PM_STUDIO_IGNORE, -1, &tr);
  }

  OcclusionFrequencyGain SteamAudioOcclusionCalculator::GetParameters(
    Vector3 listenerPosition,
    Vector3 listenerAhead,
    Vector3 listenerUp,
    Vector3 audioSourcePosition,
    float sourceRadius,
    float attenuationMultiplier
    )
  {
    if (attenuationMultiplier == ATTN_NONE)
    {
      return OcclusionFrequencyGain{ 1.0f, 1.0f, 1.0f };
    }

    auto toIPLVector3 = [](Vector3* vector) { return *reinterpret_cast<IPLVector3*>(vector); };

    IPLSource source{ toIPLVector3(&audioSourcePosition) };
    auto env = meshLoader->CurrentEnvironment();
    IPLDirectSoundPath result{};
    result = m_sa->iplGetDirectSoundPath(
      *env,
      toIPLVector3(&listenerPosition),
      toIPLVector3(&listenerAhead),
      toIPLVector3(&listenerUp),
      source,
      sourceRadius,
      SteamAudioOcclusionCalculator::NUMBER_OCCLUSION_RAYS,
      IPLDirectOcclusionMode::IPL_DIRECTOCCLUSION_TRANSMISSIONBYFREQUENCY,
      IPLDirectOcclusionMethod::IPL_DIRECTOCCLUSION_VOLUMETRIC
      );

    if (result.occlusionFactor < 0.5f) //more than half occluded, add transmission component based on obstruction length
    {
      pmtrace_s tr;

      // Convert to GoldSrc coordinates system
      auto listener_position = GoldSrc_UnpackVector(listenerPosition);
      auto audio_source_position = GoldSrc_UnpackVector(audioSourcePosition);

      // set up traceline from player eyes to sound emitting entity origin
      PlayerTrace(&listener_position, &audio_source_position, tr);

      // If hit, traceline between ent and player to get solid length.
      if ((tr.fraction < 1.0f || tr.allsolid || tr.startsolid) && tr.fraction < 0.99f)
      {
        alure::Vector3 obstruction_first_point(tr.endpos);
        PlayerTrace(&audio_source_position, &listener_position, tr);

        if ((tr.fraction < 1.0f || tr.allsolid || tr.startsolid) && tr.fraction < 0.99f && !tr.startsolid)
        {
          auto distance = obstruction_first_point.getDistance(tr.endpos) * AL_UnitToMeters;
          result.transmissionFactor[0] = alure::dBToLinear(-distance * result.transmissionFactor[0]);
          result.transmissionFactor[1] = alure::dBToLinear(-distance * result.transmissionFactor[1]);
          result.transmissionFactor[2] = alure::dBToLinear(-distance * result.transmissionFactor[2]);
        }
      }
    }

    auto getFactor = [&](size_t index) { return std::clamp((result.occlusionFactor + (1 - result.occlusionFactor) * result.transmissionFactor[index]) / attenuationMultiplier, 0.0f, 1.0f); };
    auto ret = OcclusionFrequencyGain{ getFactor(0), getFactor(1), getFactor(2) };
    return ret;
  }
}