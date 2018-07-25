#pragma once

#include "alure/AL/efx-presets.h"
#include "snd_local.h"

class EnvEffects final
{
private:
  // HL1 DSPROPERTY_EAXBUFFER_REVERBMIX seems to be always set to 0.38,
  // with no adjustment of reverb intensity with distance.
  // Reverb adjustment with distance is disabled per-source.
  const float AL_REVERBMIX = 0.38f;
  const float AL_SND_GAIN_FADE_TIME = 0.25f;

  const float AL_UNDERWATER_LP_GAIN = 0.25f;
  const float AL_UNDERWATER_DOPPLER_FACTOR_RATIO = 343.3f / 1484.0f;

  // Creative X-Fi's are buggy with the direct filter gain set to 1.0f,
  // they get stuck.
  const float gain_epsilon = 1.0f - std::numeric_limits<float>::epsilon();

  alure::AuxiliaryEffectSlot alAuxEffectSlots;

  // Effect used for interpolation
  struct
  {
    EFXEAXREVERBPROPERTIES ob_effect;        // Effect change if listener changes environment
    EFXEAXREVERBPROPERTIES ob_effect_target; // Target effect while crossfading between ob_effect and ob_effect_target
    EFXEAXREVERBPROPERTIES ob_effect_inc;    // crossfade increment
    alure::Effect generated_effect;          // Generated effect from crossfade
  } interpl_effect;

  // Default effects
  EFXEAXREVERBPROPERTIES presets_room[CSXROOM] = {
    EFX_REVERB_PRESET_GENERIC,                    //  0
    //SXROOM_GENERIC
    EFX_REVERB_PRESET_ROOM,                       //  1
    //SXROOM_METALIC_S
    EFX_REVERB_PRESET_BATHROOM,                   //  2
    EFX_REVERB_PRESET_BATHROOM,                   //  3
    EFX_REVERB_PRESET_BATHROOM,                   //  4
    //SXROOM_TUNNEL_S
    EFX_REVERB_PRESET_SEWERPIPE,                  //  4
    EFX_REVERB_PRESET_SEWERPIPE,                  //  6
    EFX_REVERB_PRESET_SEWERPIPE,                  //  7
    //SXROOM_CHAMBER_S
    EFX_REVERB_PRESET_STONEROOM,                  //  8
    EFX_REVERB_PRESET_STONEROOM,                  //  9
    EFX_REVERB_PRESET_STONEROOM,                  // 10
    //SXROOM_BRITE_S
    EFX_REVERB_PRESET_STONECORRIDOR,              // 11
    EFX_REVERB_PRESET_STONECORRIDOR,              // 12
    EFX_REVERB_PRESET_STONECORRIDOR,              // 13
    //SXROOM_WATER1
    EFX_REVERB_PRESET_UNDERWATER,                 // 14
    EFX_REVERB_PRESET_UNDERWATER,                 // 15
    EFX_REVERB_PRESET_UNDERWATER,                 // 16
    //SXROOM_CONCRETE_S
    EFX_REVERB_PRESET_GENERIC,                    // 17
    EFX_REVERB_PRESET_GENERIC,                    // 18
    EFX_REVERB_PRESET_GENERIC,                    // 19
    //SXROOM_OUTSIDE1
    EFX_REVERB_PRESET_ARENA,                      // 20
    EFX_REVERB_PRESET_ARENA,                      // 21
    EFX_REVERB_PRESET_ARENA,                      // 22
    //SXROOM_CAVERN_S
    EFX_REVERB_PRESET_CONCERTHALL,                // 23
    EFX_REVERB_PRESET_CONCERTHALL,                // 24
    EFX_REVERB_PRESET_CONCERTHALL,                // 25
    //SXROOM_WEIRDO1
    EFX_REVERB_PRESET_DIZZY,                      // 26
    EFX_REVERB_PRESET_DIZZY,                      // 27
    EFX_REVERB_PRESET_DIZZY                       // 28
};

  // For occlusion
  void PlayerTrace(vec3_t start, vec3_t end, int flags, pmtrace_s& tr);
  float FadeToNewGain(aud_channel_t *ch, float gain_new);
  float GetGainObscured(aud_channel_t *ch, cl_entity_t *pent, cl_entity_t *sent);

  // For effect interpolation
  EFXEAXREVERBPROPERTIES FadeToNewEffect(EFXEAXREVERBPROPERTIES& effect_new);

public:
  EnvEffects(alure::Context al_context);
  ~EnvEffects();

  void InterplEffect(int roomtype);
  void ApplyEffect(aud_channel_t *ch, qboolean underwater);
};