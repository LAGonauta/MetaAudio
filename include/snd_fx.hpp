#pragma once

#include "efx-presets.h"

class EnvEffects final
{
private:

  struct effectSlot
  {
    alure::AuxiliaryEffectSlot slot;
    alure::Effect effect;
    float gain_current{ 0 };
    float gain_elapsed_time{ 0 };
    float gain_initial_value{ 0 };
    float gain_old_target{ 0 };
    float gain_target{ 0 };
    effectSlot(alure::AuxiliaryEffectSlot _slot, alure::Effect _effect)
    {
      slot = _slot;
      effect = _effect;
    };
  };

  alure::Vector<effectSlot> alAuxEffectSlots;

  // Default effects
  alure::Array<EFXEAXREVERBPROPERTIES, CSXROOM> presets_room =
  { {
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
  } };

  // For occlusion
  void PlayerTrace(vec3_t start, vec3_t end, int flags, pmtrace_s& tr);
  float EnvEffects::FadeToNewValue(const bool fade_enabled,
    const bool force_final,
    float& elapsed_time,
    float& initial_value,
    const float current_value,
    float& old_final_value,
    const float final_value);
  float Lerp(float initial_value, float final_value, float fraction);
  float GetGainObscured(aud_channel_t *ch, cl_entity_t *pent, cl_entity_t *sent);

public:
  EnvEffects(alure::Context al_context, ALCuint max_sends);
  ~EnvEffects();

  void InterplEffect(int roomtype);
  void ApplyEffect(aud_channel_t *ch, qboolean underwater);
};