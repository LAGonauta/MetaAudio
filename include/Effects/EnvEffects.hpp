#pragma once

#include "Effects/IOcclusionCalculator.hpp"
#include "Utilities/Fade.hpp"
#include "Workarounds/IWorkarounds.hpp"
#include "Loaders/SteamAudioMapMeshLoader.hpp"
#include "efx-presets.h"

namespace MetaAudio
{
  class EnvEffects final
  {
  private:

    struct effectSlot
    {
      alure::AutoObj<alure::AuxiliaryEffectSlot> slot;
      alure::AutoObj<alure::Effect> effect;
      GainFading gain;
      effectSlot(alure::AuxiliaryEffectSlot slot, alure::Effect effect)
          : slot(alure::MakeAuto(slot)),
            effect(alure::MakeAuto(effect))
      {
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
    std::pair<alure::Vector3, alure::Vector3> listener_orientation;
    std::shared_ptr<IOcclusionCalculator> occlusion_calculator;
    std::unique_ptr<Fade> fader;
    std::unique_ptr<IWorkarounds> workarounds;
    void EnvEffects::FadeToNewValue(const bool fade_enabled, const bool force_final, GainFading& value);

    void ConfigureDefaultEffects();
    void OverrideEffects();

  public:
    EnvEffects(alure::Context& al_context, ALCuint max_sends, std::shared_ptr<IOcclusionCalculator> occlusion_calculator);

    void InterplEffect(int roomtype);
    void ApplyEffect(aud_channel_t* ch, qboolean underwater);
    void SetListenerOrientation(std::pair<alure::Vector3, alure::Vector3> listenerOrientation);
    void SetOccluder(std::shared_ptr<IOcclusionCalculator> occlusion_calculator);
  };
}