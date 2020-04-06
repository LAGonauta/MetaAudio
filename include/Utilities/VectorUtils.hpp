#pragma once

#include "alure2.h"

namespace MetaAudio
{
  // translates from AL coordinate system to quake
  // HL seems to use inches, convert to meters.
  static constexpr float AL_UnitToMeters = 0.0254f;

  static constexpr alure::Vector3 AL_UnpackVector(float* vector)
  {
    return { -vector[1] * AL_UnitToMeters, vector[2] * AL_UnitToMeters, -vector[0] * AL_UnitToMeters };
  }

  static constexpr alure::Vector3 AL_CopyVector(float* vector)
  {
    return { -vector[1], vector[2], -vector[0] };
  }

  static constexpr void AL_CopyVector(float* from, float* to)
  {
    to[0] = -from[1];
    to[1] = from[2];
    to[2] = -from[0];
  }

  static constexpr void VectorCopy(float* from, float* to)
  {
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];
  }
}
