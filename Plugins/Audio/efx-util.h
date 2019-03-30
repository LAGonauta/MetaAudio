#pragma once
#include <cmath>
#include "alure/AL/efx-presets.h"

inline void SX_abs(EFXEAXREVERBPROPERTIES& effect)
{
  effect.flDensity = std::abs(effect.flDensity);
  effect.flDiffusion = std::abs(effect.flDiffusion);
  effect.flGain = std::abs(effect.flGain);
  effect.flGainHF = std::abs(effect.flGainHF);
  effect.flGainLF = std::abs(effect.flGainLF);
  effect.flDecayTime = std::abs(effect.flDecayTime);
  effect.flDecayHFRatio = std::abs(effect.flDecayHFRatio);
  effect.flDecayLFRatio = std::abs(effect.flDecayLFRatio);
  effect.flReflectionsGain = std::abs(effect.flReflectionsGain);
  effect.flReflectionsDelay = std::abs(effect.flReflectionsDelay);
  effect.flLateReverbGain = std::abs(effect.flLateReverbGain);
  effect.flLateReverbDelay = std::abs(effect.flLateReverbDelay);
  effect.flEchoTime = std::abs(effect.flEchoTime);
  effect.flEchoDepth = std::abs(effect.flEchoDepth);
  effect.flModulationTime = std::abs(effect.flModulationTime);
  effect.flModulationDepth = std::abs(effect.flModulationDepth);
  effect.flAirAbsorptionGainHF = std::abs(effect.flAirAbsorptionGainHF);
  effect.flHFReference = std::abs(effect.flHFReference);
  effect.flLFReference = std::abs(effect.flLFReference);
  effect.flRoomRolloffFactor = std::abs(effect.flRoomRolloffFactor);
}

inline EFXEAXREVERBPROPERTIES SX_SubtractEffect(const EFXEAXREVERBPROPERTIES& effect1, const EFXEAXREVERBPROPERTIES& effect2)
{
  EFXEAXREVERBPROPERTIES output;
  output.flDensity = effect1.flDensity - effect2.flDensity;
  output.flDiffusion = effect1.flDiffusion - effect2.flDiffusion;
  output.flGain = effect1.flGain - effect2.flGain;
  output.flGainHF = effect1.flGainHF - effect2.flGainHF;
  output.flGainLF = effect1.flGainLF - effect2.flGainLF;
  output.flDecayTime = effect1.flDecayTime - effect2.flDecayTime;
  output.flDecayHFRatio = effect1.flDecayHFRatio - effect2.flDecayHFRatio;
  output.flDecayLFRatio = effect1.flDecayLFRatio - effect2.flDecayLFRatio;
  output.flReflectionsGain = effect1.flReflectionsGain - effect2.flReflectionsGain;
  output.flReflectionsDelay = effect1.flReflectionsDelay - effect2.flReflectionsDelay;
  output.flLateReverbGain = effect1.flLateReverbGain - effect2.flLateReverbGain;
  output.flLateReverbDelay = effect1.flLateReverbDelay - effect2.flLateReverbDelay;
  output.flEchoTime = effect1.flEchoTime - effect2.flEchoTime;
  output.flEchoDepth = effect1.flEchoDepth - effect2.flEchoDepth;
  output.flModulationTime = effect1.flModulationTime - effect2.flModulationTime;
  output.flModulationDepth = effect1.flModulationDepth - effect2.flModulationDepth;
  output.flAirAbsorptionGainHF = effect1.flAirAbsorptionGainHF - effect2.flAirAbsorptionGainHF;
  output.flHFReference = effect1.flHFReference - effect2.flHFReference;
  output.flLFReference = effect1.flLFReference - effect2.flLFReference;
  output.flRoomRolloffFactor = effect1.flRoomRolloffFactor - effect2.flRoomRolloffFactor;
  return output;
}

inline void SX_SetEffect(EFXEAXREVERBPROPERTIES& dest, const float& value)
{
  dest.flDensity = value;
  dest.flDiffusion = value;
  dest.flGain = value;
  dest.flGainHF = value;
  dest.flGainLF = value;
  dest.flDecayTime = value;
  dest.flDecayHFRatio = value;
  dest.flDecayLFRatio = value;
  dest.flReflectionsGain = value;
  dest.flReflectionsDelay = value;
  dest.flLateReverbGain = value;
  dest.flLateReverbDelay = value;
  dest.flEchoTime = value;
  dest.flEchoDepth = value;
  dest.flModulationTime = value;
  dest.flModulationDepth = value;
  dest.flAirAbsorptionGainHF = value;
  dest.flHFReference = value;
  dest.flLFReference = value;
  dest.flRoomRolloffFactor = value;
}

inline void SX_MultiplyEffect(EFXEAXREVERBPROPERTIES& dest, const float& value)
{
  dest.flDensity *= value;
  dest.flDiffusion *= value;
  dest.flGain *= value;
  dest.flGainHF *= value;
  dest.flGainLF *= value;
  dest.flDecayTime *= value;
  dest.flDecayHFRatio *= value;
  dest.flDecayLFRatio *= value;
  dest.flReflectionsGain *= value;
  dest.flReflectionsDelay *= value;
  dest.flLateReverbGain *= value;
  dest.flLateReverbDelay *= value;
  dest.flEchoTime *= value;
  dest.flEchoDepth *= value;
  dest.flModulationTime *= value;
  dest.flModulationDepth *= value;
  dest.flAirAbsorptionGainHF *= value;
  dest.flHFReference *= value;
  dest.flLFReference *= value;
  dest.flRoomRolloffFactor *= value;
}

inline bool SX_CompareEffectDiffToValue(const EFXEAXREVERBPROPERTIES& effect1, const EFXEAXREVERBPROPERTIES& effect2, const float& value)
{
  return (std::abs(effect1.flDensity - effect2.flDensity) < value &&
    std::abs(effect1.flDiffusion - effect2.flDiffusion) < value &&
    std::abs(effect1.flGain - effect2.flGain) < value &&
    std::abs(effect1.flGainHF - effect2.flGainHF) < value &&
    std::abs(effect1.flGainLF - effect2.flGainLF) < value &&
    std::abs(effect1.flDecayTime - effect2.flDecayTime) < value &&
    std::abs(effect1.flDecayHFRatio - effect2.flDecayHFRatio) < value &&
    std::abs(effect1.flDecayLFRatio - effect2.flDecayLFRatio) < value &&
    std::abs(effect1.flReflectionsGain - effect2.flReflectionsGain) < value &&
    std::abs(effect1.flReflectionsDelay - effect2.flReflectionsDelay) < value &&
    std::abs(effect1.flLateReverbGain - effect2.flLateReverbGain) < value &&
    std::abs(effect1.flLateReverbDelay - effect2.flLateReverbDelay) < value &&
    std::abs(effect1.flEchoTime - effect2.flEchoTime) < value &&
    std::abs(effect1.flEchoDepth - effect2.flEchoDepth) < value &&
    std::abs(effect1.flModulationTime - effect2.flModulationTime) < value &&
    std::abs(effect1.flModulationDepth - effect2.flModulationDepth) < value &&
    std::abs(effect1.flAirAbsorptionGainHF - effect2.flAirAbsorptionGainHF) < value &&
    std::abs(effect1.flHFReference - effect2.flHFReference) < value &&
    std::abs(effect1.flLFReference - effect2.flLFReference) < value &&
    std::abs(effect1.flRoomRolloffFactor - effect2.flRoomRolloffFactor) < value);
}

inline float ApproachVal(float target, float value, float speed)
{
  float delta = target - value;

  if (delta > speed)
    value += speed;
  else if (delta < -speed)
    value -= speed;
  else value = target;

  return value;
}

inline void SX_Approach(float& ob_gain, const float& ob_gain_target, const float& ob_gain_inc, const float& value)
{
  // if not hit target, keep approaching
  if (std::abs(ob_gain - ob_gain_target) > value)
  {
    ob_gain = ApproachVal(ob_gain_target, ob_gain, ob_gain_inc);
  }
  else
  {
    // close enough, set gain = target
    ob_gain = ob_gain_target;
  }
}

inline void SX_ApproachEffect(EFXEAXREVERBPROPERTIES& ob_gain, const EFXEAXREVERBPROPERTIES& ob_gain_target, const  EFXEAXREVERBPROPERTIES& ob_gain_inc, const float& value)
{
  SX_Approach(ob_gain.flDensity, ob_gain_target.flDensity, ob_gain_inc.flDensity, value);
  SX_Approach(ob_gain.flDiffusion, ob_gain_target.flDiffusion, ob_gain_inc.flDiffusion, value);
  SX_Approach(ob_gain.flGain, ob_gain_target.flGain, ob_gain_inc.flGain, value);
  SX_Approach(ob_gain.flGainHF, ob_gain_target.flGainHF, ob_gain_inc.flGainHF, value);
  SX_Approach(ob_gain.flGainLF, ob_gain_target.flGainLF, ob_gain_inc.flGainLF, value);
  SX_Approach(ob_gain.flDecayTime, ob_gain_target.flDecayTime, ob_gain_inc.flDecayTime, value);
  SX_Approach(ob_gain.flDecayHFRatio, ob_gain_target.flDecayHFRatio, ob_gain_inc.flDecayHFRatio, value);
  SX_Approach(ob_gain.flDecayLFRatio, ob_gain_target.flDecayLFRatio, ob_gain_inc.flDecayLFRatio, value);
  SX_Approach(ob_gain.flReflectionsGain, ob_gain_target.flReflectionsGain, ob_gain_inc.flReflectionsGain, value);
  SX_Approach(ob_gain.flReflectionsDelay, ob_gain_target.flReflectionsDelay, ob_gain_inc.flReflectionsDelay, value);
  SX_Approach(ob_gain.flLateReverbGain, ob_gain_target.flLateReverbGain, ob_gain_inc.flLateReverbGain, value);
  SX_Approach(ob_gain.flLateReverbDelay, ob_gain_target.flLateReverbDelay, ob_gain_inc.flLateReverbDelay, value);
  SX_Approach(ob_gain.flEchoTime, ob_gain_target.flEchoTime, ob_gain_inc.flEchoTime, value);
  SX_Approach(ob_gain.flEchoDepth, ob_gain_target.flEchoDepth, ob_gain_inc.flEchoDepth, value);
  SX_Approach(ob_gain.flModulationTime, ob_gain_target.flModulationTime, ob_gain_inc.flModulationTime, value);
  SX_Approach(ob_gain.flModulationDepth, ob_gain_target.flModulationDepth, ob_gain_inc.flModulationDepth, value);
  SX_Approach(ob_gain.flAirAbsorptionGainHF, ob_gain_target.flAirAbsorptionGainHF, ob_gain_inc.flAirAbsorptionGainHF, value);
  SX_Approach(ob_gain.flHFReference, ob_gain_target.flHFReference, ob_gain_inc.flHFReference, value);
  SX_Approach(ob_gain.flLFReference, ob_gain_target.flLFReference, ob_gain_inc.flLFReference, value);
  SX_Approach(ob_gain.flRoomRolloffFactor, ob_gain_target.flRoomRolloffFactor, ob_gain_inc.flRoomRolloffFactor, value);
}
