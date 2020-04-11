#include <metahook.h>

#include "Effects/GoldSrcOcclusionCalculator.hpp"

#include "alure2.h"
#include "pm_defs.h"

namespace MetaAudio
{
  GoldSrcOcclusionCalculator::GoldSrcOcclusionCalculator(event_api_s* event_api)
  {
    this->event_api = event_api;
  }

  void GoldSrcOcclusionCalculator::PlayerTrace(Vector3 start, Vector3 end, pmtrace_s& tr)
  {
    // 0 = regular player hull, 1 = ducked player hull, 2 = point hull
    event_api->EV_SetTraceHull(2);
    event_api->EV_PlayerTrace(reinterpret_cast<float*>(&start), reinterpret_cast<float*>(&end), PM_STUDIO_IGNORE, -1, &tr);
  }

  OcclusionFilter GoldSrcOcclusionCalculator::GetParameters(
    Vector3 listenerPosition,
    Vector3 listenerAhead,
    Vector3 listenerUp,
    Vector3 audioSourcePosition,
    float sourceRadius,
    float attenuationMultiplier
    )
  {
    float gain = 1.0f;

    if (attenuationMultiplier)
    {
      pmtrace_s tr;

      // set up traceline from player eyes to sound emitting entity origin
      PlayerTrace(listenerPosition, audioSourcePosition, tr);

      // If hit, traceline between ent and player to get solid length.
      if ((tr.fraction < 1.0f || tr.allsolid || tr.startsolid) && tr.fraction < 0.99f)
      {
        alure::Vector3 obstruction_first_point(tr.endpos);
        PlayerTrace(audioSourcePosition, listenerPosition, tr);

        if ((tr.fraction < 1.0f || tr.allsolid || tr.startsolid) && tr.fraction < 0.99f && !tr.startsolid)
        {
          gain = gain * alure::dBToLinear(TRANSMISSION_ATTN_PER_INCH * attenuationMultiplier * obstruction_first_point.getDistance(tr.endpos));
        }
      }
    }

    return OcclusionFilter{ gain, gain, gain };
  }
}