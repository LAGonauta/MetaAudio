#pragma once
#include <math.h>
#include "util.h"
#include "alure/AL/efx-presets.h"

inline void SX_CopyEffect(EFXEAXREVERBPROPERTIES& dest, EFXEAXREVERBPROPERTIES& source)
{
  dest.flDensity = source.flDensity;
  dest.flDiffusion = source.flDiffusion;
  dest.flGain = source.flGain;
  dest.flGainHF = source.flGainHF;
  dest.flGainLF = source.flGainLF;
  dest.flDecayTime = source.flDecayTime;
  dest.flDecayHFRatio = source.flDecayHFRatio;
  dest.flDecayLFRatio = source.flDecayLFRatio;
  dest.flReflectionsGain = source.flReflectionsGain;
  dest.flReflectionsDelay = source.flReflectionsDelay;
  dest.flLateReverbGain = source.flLateReverbGain;
  dest.flLateReverbDelay = source.flLateReverbDelay;
  dest.flEchoTime = source.flEchoTime;
  dest.flEchoDepth = source.flEchoDepth;
  dest.flModulationTime = source.flModulationTime;
  dest.flModulationDepth = source.flModulationDepth;
  dest.flAirAbsorptionGainHF = source.flAirAbsorptionGainHF;
  dest.flHFReference = source.flHFReference;
  dest.flLFReference = source.flLFReference;
  dest.flRoomRolloffFactor = source.flRoomRolloffFactor;
}

inline void SX_fabs(EFXEAXREVERBPROPERTIES& effect)
{
  effect.flDensity = fabs(effect.flDensity);
  effect.flDiffusion = fabs(effect.flDiffusion);
  effect.flGain = fabs(effect.flGain);
  effect.flGainHF = fabs(effect.flGainHF);
  effect.flGainLF = fabs(effect.flGainLF);
  effect.flDecayTime = fabs(effect.flDecayTime);
  effect.flDecayHFRatio = fabs(effect.flDecayHFRatio);
  effect.flDecayLFRatio = fabs(effect.flDecayLFRatio);
  effect.flReflectionsGain = fabs(effect.flReflectionsGain);
  effect.flReflectionsDelay = fabs(effect.flReflectionsDelay);
  effect.flLateReverbGain = fabs(effect.flLateReverbGain);
  effect.flLateReverbDelay = fabs(effect.flLateReverbDelay);
  effect.flEchoTime = fabs(effect.flEchoTime);
  effect.flEchoDepth = fabs(effect.flEchoDepth);
  effect.flModulationTime = fabs(effect.flModulationTime);
  effect.flModulationDepth = fabs(effect.flModulationDepth);
  effect.flAirAbsorptionGainHF = fabs(effect.flAirAbsorptionGainHF);
  effect.flHFReference = fabs(effect.flHFReference);
  effect.flLFReference = fabs(effect.flLFReference);
  effect.flRoomRolloffFactor = fabs(effect.flRoomRolloffFactor);
}

inline EFXEAXREVERBPROPERTIES SX_SubtractEffect(EFXEAXREVERBPROPERTIES& effect1, EFXEAXREVERBPROPERTIES& effect2)
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

inline void SX_SetEffect(EFXEAXREVERBPROPERTIES& dest, float value)
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

inline void SX_MultiplyEffect(EFXEAXREVERBPROPERTIES& dest, float value)
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

inline bool SX_CompareEffectDiffToValue(EFXEAXREVERBPROPERTIES& effect1, EFXEAXREVERBPROPERTIES& effect2, float value)
{
  return (fabs(effect1.flDensity - effect2.flDensity) < value &&
    fabs(effect1.flDiffusion - effect2.flDiffusion) < value &&
    fabs(effect1.flGain - effect2.flGain) < value &&
    fabs(effect1.flGainHF - effect2.flGainHF) < value &&
    fabs(effect1.flGainLF - effect2.flGainLF) < value &&
    fabs(effect1.flDecayTime - effect2.flDecayTime) < value &&
    fabs(effect1.flDecayHFRatio - effect2.flDecayHFRatio) < value &&
    fabs(effect1.flDecayLFRatio - effect2.flDecayLFRatio) < value &&
    fabs(effect1.flReflectionsGain - effect2.flReflectionsGain) < value &&
    fabs(effect1.flReflectionsDelay - effect2.flReflectionsDelay) < value &&
    fabs(effect1.flLateReverbGain - effect2.flLateReverbGain) < value &&
    fabs(effect1.flLateReverbDelay - effect2.flLateReverbDelay) < value &&
    fabs(effect1.flEchoTime - effect2.flEchoTime) < value &&
    fabs(effect1.flEchoDepth - effect2.flEchoDepth) < value &&
    fabs(effect1.flModulationTime - effect2.flModulationDepth) < value &&
    fabs(effect1.flModulationDepth - effect2.flModulationDepth) < value &&
    fabs(effect1.flAirAbsorptionGainHF - effect2.flAirAbsorptionGainHF) < value &&
    fabs(effect1.flHFReference - effect2.flHFReference) < value &&
    fabs(effect1.flLFReference - effect2.flLFReference) < value &&
    fabs(effect1.flRoomRolloffFactor - effect2.flRoomRolloffFactor) < value);
}

inline void SX_Approach(float& ob_gain, float& ob_gain_target, float& ob_gain_inc, float& value)
{
  // if not hit target, keep approaching
  if (fabs(ob_gain - ob_gain_target) > value)
  {
    ob_gain = ApproachVal(ob_gain_target, ob_gain, ob_gain_inc);
  }
  else
  {
    // close enough, set gain = target
    ob_gain = ob_gain_target;
  }
}

inline void SX_ApproachEffect(EFXEAXREVERBPROPERTIES& ob_gain, EFXEAXREVERBPROPERTIES& ob_gain_target, EFXEAXREVERBPROPERTIES& ob_gain_inc, float value)
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
