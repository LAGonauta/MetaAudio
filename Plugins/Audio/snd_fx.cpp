#include <metahook.h>
#include "exportfuncs.h"
#include "FileSystem.h"
#include "util.h"
#include "snd_local.h"
#include <math.h>
#include "alure/AL/efx-presets.h"

extern cvar_t *sxroom_off;
extern cvar_t *sxroomwater_type;
extern cvar_t *sxroom_type;

alure::Effect alReverbEffects[CSXROOM];
alure::AuxiliaryEffectSlot alAuxEffectSlots;

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

void SX_ApplyEffect(aud_channel_t *ch, int roomtype, qboolean underwater)
{
  if (roomtype > 0 && roomtype < CSXROOM && sxroom_off && !sxroom_off->value)
  {
    if (underwater)
    {
      alAuxEffectSlots.applyEffect(alReverbEffects[14]);
      ch->source.setAuxiliarySend(alAuxEffectSlots, 0);
      ch->source.setDirectFilter(alure::FilterParams{ 1.0f, 0.25f, AL_HIGHPASS_DEFAULT_GAIN });
    }
    else
    {
      alAuxEffectSlots.applyEffect(alReverbEffects[roomtype]);
      ch->source.setAuxiliarySend(alAuxEffectSlots, 0);
      ch->source.setDirectFilter(alure::FilterParams{ 1.0f, AL_LOWPASS_DEFAULT_GAIN, AL_HIGHPASS_DEFAULT_GAIN });
    }
  }
  else
  {
    alAuxEffectSlots.applyEffect(alReverbEffects[0]);
    ch->source.setAuxiliarySend(alAuxEffectSlots, 0);

    if (underwater)
      ch->source.setDirectFilter(alure::FilterParams{ 1.0f, 0.25, AL_HIGHPASS_DEFAULT_GAIN });
    else
      ch->source.setDirectFilter(alure::FilterParams{ 1.0f, AL_LOWPASS_DEFAULT_GAIN, AL_HIGHPASS_DEFAULT_GAIN });
  }
}

void SX_Init(void)
{
  alure::Context al_context = alure::Context::GetCurrent();

  // Disable reverb when room_type = 0:
  presets_room[0].flGain = 0;

  // HL uses EAX 1.0, which are really different from non-EAX reverbs.
  // flGain = EAX 1.0 volume
  // flDecayTime = EAX 1.0 decay time
  // flDecayHFRatio = EAX 1.0 damping (probably)
  presets_room[1].flGain = 0.417;
  presets_room[1].flDecayTime = 0.4;
  presets_room[1].flDecayHFRatio = 2 / 3;

  presets_room[2].flGain = 0.3;
  presets_room[2].flDecayTime = 1.5;
  presets_room[2].flDecayHFRatio = 1 / 6;

  presets_room[3].flGain = 0.4;
  presets_room[3].flDecayTime = 1.5;
  presets_room[3].flDecayHFRatio = 1 / 6;

  presets_room[4].flGain = 0.6;
  presets_room[4].flDecayTime = 1.5;
  presets_room[4].flDecayHFRatio = 1 / 6;

  presets_room[5].flGain = 0.4;
  presets_room[5].flDecayTime = 2.886;
  presets_room[5].flDecayHFRatio = 0.25;

  presets_room[6].flGain = 0.6;
  presets_room[6].flDecayTime = 2.886;
  presets_room[6].flDecayHFRatio = 0.25;

  presets_room[7].flGain = 0.8;
  presets_room[7].flDecayTime = 2.886;
  presets_room[7].flDecayHFRatio = 0.25;

  presets_room[8].flGain = 0.5;
  presets_room[8].flDecayTime = 2.309;
  presets_room[8].flDecayHFRatio = 0.888;

  presets_room[9].flGain = 0.65;
  presets_room[9].flDecayTime = 2.309;
  presets_room[9].flDecayHFRatio = 0.888;

  presets_room[10].flGain = 0.8;
  presets_room[10].flDecayTime = 2.309;
  presets_room[10].flDecayHFRatio = 0.888;

  presets_room[11].flGain = 0.3;
  presets_room[11].flDecayTime = 2.697;
  presets_room[11].flDecayHFRatio = 0.638;

  presets_room[12].flGain = 0.5;
  presets_room[11].flDecayTime = 2.697;
  presets_room[11].flDecayHFRatio = 0.638;

  presets_room[13].flGain = 0.65;
  presets_room[11].flDecayTime = 2.697;
  presets_room[11].flDecayHFRatio = 0.638;

  presets_room[14].flGain = 1;
  presets_room[14].flDecayTime = 1.5;
  presets_room[14].flDecayHFRatio = 0;

  presets_room[15].flGain = 1;
  presets_room[15].flDecayTime = 2.5;
  presets_room[15].flDecayHFRatio = 0;

  presets_room[16].flGain = 1;
  presets_room[16].flDecayTime = 3.5;
  presets_room[16].flDecayHFRatio = 0;

  presets_room[17].flGain = 0.65;
  presets_room[17].flDecayTime = 1.493;
  presets_room[17].flDecayHFRatio = 0.5;

  presets_room[18].flGain = 0.85;
  presets_room[18].flDecayTime = 1.493;
  presets_room[18].flDecayHFRatio = 0.5;

  presets_room[19].flGain = 1;
  presets_room[19].flDecayTime = 1.493;
  presets_room[19].flDecayHFRatio = 0.5;

  presets_room[20].flGain = 0.4;
  presets_room[20].flDecayTime = 7.284;
  presets_room[20].flDecayHFRatio = 1 / 3;

  presets_room[21].flGain = 0.55;
  presets_room[21].flDecayTime = 7.284;
  presets_room[21].flDecayHFRatio = 1 / 3;

  presets_room[22].flGain = 0.7;
  presets_room[22].flDecayTime = 7.284;
  presets_room[22].flDecayHFRatio = 1 / 3;

  presets_room[23].flGain = 0.5;
  presets_room[23].flDecayTime = 3.961;
  presets_room[23].flDecayHFRatio = 0.5;

  presets_room[24].flGain = 0.7;
  presets_room[24].flDecayTime = 3.961;
  presets_room[24].flDecayHFRatio = 0.5;

  presets_room[25].flGain = 1;
  presets_room[25].flDecayTime = 3.961;
  presets_room[25].flDecayHFRatio = 0.5;

  presets_room[26].flGain = 0.2;
  presets_room[26].flDecayTime = 17.234;
  presets_room[26].flDecayHFRatio = 2 / 3;

  presets_room[27].flGain = 0.3;
  presets_room[27].flDecayTime = 17.234;
  presets_room[27].flDecayHFRatio = 2 / 3;

  presets_room[28].flGain = 0.4;
  presets_room[28].flDecayTime = 17.234;
  presets_room[28].flDecayHFRatio = 2 / 3;

  // Init each effect
  for (int i = 0; i < CSXROOM; ++i)
  {
    presets_room[i].flRoomRolloffFactor = 1.0f;
    alReverbEffects[i] = al_context.createEffect();
    alReverbEffects[i].setReverbProperties(presets_room[i]);
  }

  alAuxEffectSlots = al_context.createAuxiliaryEffectSlot();
}

void SX_Shutdown(void)
{
  for (size_t i = 0; i < CSXROOM; ++i)
  {
    alReverbEffects[i].destroy();
  }
  alAuxEffectSlots.destroy();
}