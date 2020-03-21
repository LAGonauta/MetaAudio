#include <metahook.h>
#include <filesystem>

#include "event_api.h"
#include "pm_defs.h"
#include "snd_local.h"
#include "snd_fx.hpp"
#include "snd_efx_reader.hpp"

extern cvar_t *sxroom_off;
extern cvar_t *sxroomwater_type;
extern cvar_t *sxroom_type;
static cvar_t *al_occlusion = nullptr;
static cvar_t *al_occlusion_fade = nullptr;

// HL1 DSPROPERTY_EAXBUFFER_REVERBMIX seems to be always set to 0.38,
// with no adjustment of reverb intensity with distance.
// Reverb adjustment with distance is disabled per-source.
static constexpr float AL_REVERBMIX = 0.38f;
static constexpr float AL_SND_GAIN_FADE_TIME = 0.25f;

static constexpr float AL_UNDERWATER_LP_GAIN = 0.25f;
static constexpr float AL_UNDERWATER_DOPPLER_FACTOR_RATIO = 343.3f / 1484.0f;

// Creative X-Fi's are buggy with the direct filter gain set to 1.0f,
// they get stuck.
static constexpr float gain_epsilon = 1.0f - std::numeric_limits<float>::epsilon();

void EnvEffects::PlayerTrace(vec3_t start, vec3_t end, int flags, pmtrace_s& tr)
{
  // 0 = regular player hull, 1 = ducked player hull, 2 = point hull
  gEngfuncs.pEventAPI->EV_SetTraceHull(2);
  gEngfuncs.pEventAPI->EV_PlayerTrace(start, end, flags, -1, &tr);
}

float EnvEffects::Lerp(float inital_value, float final_value, float fraction)
{
  return (inital_value * (1.0f - fraction)) + (final_value * fraction);
}

float EnvEffects::FadeToNewValue(const bool fade_enabled,
  const bool force_final,
  float& elapsed_time,
  float& initial_value,
  const float current_value,
  float& old_final_value,
  const float final_value)
{
  if (fade_enabled == false || force_final)
  {
    return final_value;
  }

  if (old_final_value != final_value)
  {
    elapsed_time = 0;
    initial_value = current_value;
    old_final_value = final_value;
  }

  float frametime = static_cast<float>((*gAudEngine.cl_time) - (*gAudEngine.cl_oldtime));
  if (frametime == 0.0f)
  {
    return current_value;
  }
  elapsed_time += frametime;

  if (elapsed_time >= AL_SND_GAIN_FADE_TIME)
  {
    elapsed_time = AL_SND_GAIN_FADE_TIME;
    return final_value;
  }

  return Lerp(initial_value, final_value, elapsed_time / AL_SND_GAIN_FADE_TIME);
}

// Attenuate -2.7dB per meter? Probably should be more.
// Attenuation per wall inch in dB
constexpr float TRANSMISSION_ATTN_PER_INCH = -2.7f * 0.0254f;
float EnvEffects::GetGainObscured(aud_channel_t *ch, cl_entity_t *pent, cl_entity_t *sent)
{
  float gain = gain_epsilon;

  if (ch->attenuation)
  {
    pmtrace_s tr;

    // set up traceline from player eyes to sound emitting entity origin
    PlayerTrace(pent->origin, ch->origin, PM_STUDIO_IGNORE, tr);

    // If hit, traceline between ent and player to get solid length.
    if ((tr.fraction < 1.0f || tr.allsolid || tr.startsolid) && tr.fraction < 0.99f)
    {
      alure::Vector3 obstruction_first_point = tr.endpos;
      PlayerTrace(ch->origin, pent->origin, PM_STUDIO_IGNORE, tr);

      if ((tr.fraction < 1.0f || tr.allsolid || tr.startsolid) && tr.fraction < 0.99f && !tr.startsolid)
      {
        gain = gain * alure::dBToLinear(TRANSMISSION_ATTN_PER_INCH * ch->attenuation * obstruction_first_point.getDistance(tr.endpos));
      }
    }
  }

  return gain;
}

void EnvEffects::InterplEffect(int roomtype)
{
  EFXEAXREVERBPROPERTIES desired = presets_room[0];
  if (roomtype > 0 && roomtype < CSXROOM && sxroom_off && !sxroom_off->value)
  {
    desired = presets_room[roomtype];
  }

  static int effect_slot = 0;
  static int room_type = 0;

  if (alAuxEffectSlots.size() > 1)
  {
    if (room_type == roomtype)
    {
      alAuxEffectSlots[effect_slot].gain_target = AL_REVERBMIX;
      alAuxEffectSlots[effect_slot].gain_current =
        FadeToNewValue(true, false,
          alAuxEffectSlots[effect_slot].gain_elapsed_time,
          alAuxEffectSlots[effect_slot].gain_initial_value,
          alAuxEffectSlots[effect_slot].gain_current,
          alAuxEffectSlots[effect_slot].gain_old_target,
          alAuxEffectSlots[effect_slot].gain_target);

      auto other_effect_slot = (effect_slot + 1) % alAuxEffectSlots.size();
      alAuxEffectSlots[other_effect_slot].gain_target = 0.0f;
      alAuxEffectSlots[other_effect_slot].gain_current =
        FadeToNewValue(true, false,
          alAuxEffectSlots[other_effect_slot].gain_elapsed_time,
          alAuxEffectSlots[other_effect_slot].gain_initial_value,
          alAuxEffectSlots[other_effect_slot].gain_current,
          alAuxEffectSlots[other_effect_slot].gain_old_target,
          alAuxEffectSlots[other_effect_slot].gain_target);
    }
    else
    {
      room_type = roomtype;

      alAuxEffectSlots[effect_slot].gain_target = 0.0f;
      alAuxEffectSlots[effect_slot].gain_current =
        FadeToNewValue(true, false,
          alAuxEffectSlots[effect_slot].gain_elapsed_time,
          alAuxEffectSlots[effect_slot].gain_initial_value,
          alAuxEffectSlots[effect_slot].gain_current,
          alAuxEffectSlots[effect_slot].gain_old_target,
          alAuxEffectSlots[effect_slot].gain_target);

      effect_slot = (effect_slot + 1) % alAuxEffectSlots.size();

      alAuxEffectSlots[effect_slot].gain_target = AL_REVERBMIX;
      alAuxEffectSlots[effect_slot].gain_current =
        FadeToNewValue(true, false,
          alAuxEffectSlots[effect_slot].gain_elapsed_time,
          alAuxEffectSlots[effect_slot].gain_initial_value,
          alAuxEffectSlots[effect_slot].gain_current,
          alAuxEffectSlots[effect_slot].gain_old_target,
          alAuxEffectSlots[effect_slot].gain_target);

      alAuxEffectSlots[effect_slot].effect.setReverbProperties(desired);
    }
  }
  else if (alAuxEffectSlots.size() == 1)
  {
    alAuxEffectSlots[0].effect.setReverbProperties(desired);
  }

  for (auto& effectSlot : alAuxEffectSlots)
  {
    effectSlot.slot.setGain(effectSlot.gain_current);
    effectSlot.slot.applyEffect(effectSlot.effect);
  }
}

void EnvEffects::ApplyEffect(aud_channel_t *ch, qboolean underwater)
{
  float direct_gain = gain_epsilon;
  cl_entity_t *pent = gEngfuncs.GetEntityByIndex(*gAudEngine.cl_viewentity);
  cl_entity_t *sent = gEngfuncs.GetEntityByIndex(ch->entnum);
  if (ch->entnum != *gAudEngine.cl_viewentity && pent != nullptr && sent != nullptr)
  {
    // Detect collisions and reduce gain on occlusion
    if (al_occlusion->value)
    {
      // Check occlusion only on those entities that can be heard.
      float distance = alure::Vector3(ch->origin[0], ch->origin[1], ch->origin[2]).getDistanceSquared(
        alure::Vector3(pent->origin[0], pent->origin[1], pent->origin[2]));
      float zero_gain_distance = FLT_MAX;
      if (ch->attenuation)
      {
        zero_gain_distance = (1000.0f / ch->attenuation) * (1000.0f / ch->attenuation);
      }

      if (distance < zero_gain_distance)
      {
        ch->ob_gain_target = GetGainObscured(ch, pent, sent);
      }
    }
    else
    {
      ch->ob_gain_target = direct_gain;
    }

    ch->ob_gain_current = FadeToNewValue(al_occlusion_fade->value,
      ch->firstpass,
      ch->ob_gain_elapsed_time,
      ch->ob_gain_initial_value,
      ch->ob_gain_current,
      ch->ob_gain_old_target,
      ch->ob_gain_target);
    direct_gain = ch->ob_gain_current;
  }

  if (underwater)
  {
    ch->source.setDirectFilter(alure::FilterParams{ direct_gain, AL_UNDERWATER_LP_GAIN * direct_gain, AL_HIGHPASS_DEFAULT_GAIN });
    ch->source.setDopplerFactor(AL_UNDERWATER_DOPPLER_FACTOR_RATIO);
  }
  else
  {
    ch->source.setDirectFilter(alure::FilterParams{ direct_gain, direct_gain, AL_HIGHPASS_DEFAULT_GAIN });
    ch->source.setDopplerFactor(1.0f);
  }

  for (size_t i = 0; i < alAuxEffectSlots.size(); ++i)
  {
    ch->source.setAuxiliarySendFilter(alAuxEffectSlots[i].slot, i, alure::FilterParams{ direct_gain, direct_gain, AL_HIGHPASS_DEFAULT_GAIN });
  }
}

EnvEffects::EnvEffects(alure::Context al_context, ALCuint max_sends)
{
  if (al_occlusion == nullptr)
  {
    al_occlusion = gEngfuncs.pfnRegisterVariable("al_occlusion", "1", FCVAR_EXTDLL);
  }

  if (al_occlusion_fade == nullptr)
  {
    al_occlusion_fade = gEngfuncs.pfnRegisterVariable("al_occlusion_fade", "1", FCVAR_EXTDLL);
  }

  const char* _al_maxsends;
  CommandLine()->CheckParm("-al_maxsends", &_al_maxsends);

  if (_al_maxsends != nullptr)
  {
    auto sends = std::atoi(_al_maxsends);
    if (sends >= 0 && sends < max_sends)
    {
      max_sends = sends;
    }
  }

  ConfigureDefaultEffects();

  OverrideEffects();

  if (max_sends > 1)
  {
    alAuxEffectSlots.emplace_back(al_context.createAuxiliaryEffectSlot(), al_context.createEffect());
    alAuxEffectSlots.emplace_back(al_context.createAuxiliaryEffectSlot(), al_context.createEffect());
  }
  else if (max_sends == 1)
  {
    alAuxEffectSlots.emplace_back(al_context.createAuxiliaryEffectSlot(), al_context.createEffect());
  }

  if (alAuxEffectSlots.size() > 0)
  {
    alAuxEffectSlots[0].slot.setGain(AL_REVERBMIX);
    alAuxEffectSlots[0].gain_current = AL_REVERBMIX;
    alAuxEffectSlots[0].gain_initial_value = AL_REVERBMIX;
    alAuxEffectSlots[0].gain_old_target = AL_REVERBMIX;
    alAuxEffectSlots[0].gain_target = AL_REVERBMIX;

    if (alAuxEffectSlots.size() > 1)
    {
      alAuxEffectSlots[1].slot.setGain(0.0f);
    }
  }
}

void EnvEffects::ConfigureDefaultEffects()
{
  // Disable reverb when room_type = 0:
  presets_room[0].flGain = 0;

  // HL uses EAX 1.0, which are really different from non-EAX reverbs.
  // flGain = EAX 1.0 volume
  // flDecayTime = EAX 1.0 decay time
  // flDecayHFRatio = EAX 1.0 damping (probably)
  presets_room[1].flGain = 0.417f;
  presets_room[1].flDecayTime = 0.4f;
  presets_room[1].flDecayHFRatio = 2.0f / 3.0f;

  presets_room[2].flGain = 0.3f;
  presets_room[2].flDecayTime = 1.5f;
  presets_room[2].flDecayHFRatio = 1.0f / 6.0f;

  presets_room[3].flGain = 0.4f;
  presets_room[3].flDecayTime = 1.5f;
  presets_room[3].flDecayHFRatio = 1.0f / 6.0f;

  presets_room[4].flGain = 0.6f;
  presets_room[4].flDecayTime = 1.5f;
  presets_room[4].flDecayHFRatio = 1.0f / 6.0f;

  presets_room[5].flGain = 0.4f;
  presets_room[5].flDecayTime = 2.886f;
  presets_room[5].flDecayHFRatio = 0.25;

  presets_room[6].flGain = 0.6f;
  presets_room[6].flDecayTime = 2.886f;
  presets_room[6].flDecayHFRatio = 0.25f;

  presets_room[7].flGain = 0.8f;
  presets_room[7].flDecayTime = 2.886f;
  presets_room[7].flDecayHFRatio = 0.25f;

  presets_room[8].flGain = 0.5f;
  presets_room[8].flDecayTime = 2.309f;
  presets_room[8].flDecayHFRatio = 0.888f;

  presets_room[9].flGain = 0.65f;
  presets_room[9].flDecayTime = 2.309f;
  presets_room[9].flDecayHFRatio = 0.888f;

  presets_room[10].flGain = 0.8f;
  presets_room[10].flDecayTime = 2.309f;
  presets_room[10].flDecayHFRatio = 0.888f;

  presets_room[11].flGain = 0.3f;
  presets_room[11].flDecayTime = 2.697f;
  presets_room[11].flDecayHFRatio = 0.638f;

  presets_room[12].flGain = 0.5f;
  presets_room[11].flDecayTime = 2.697f;
  presets_room[11].flDecayHFRatio = 0.638f;

  presets_room[13].flGain = 0.65f;
  presets_room[11].flDecayTime = 2.697f;
  presets_room[11].flDecayHFRatio = 0.638f;

  presets_room[14].flGain = 1.0f;
  presets_room[14].flDecayTime = 1.5f;
  presets_room[14].flDecayHFRatio = 0.0f;

  presets_room[15].flGain = 1.0f;
  presets_room[15].flDecayTime = 2.5f;
  presets_room[15].flDecayHFRatio = 0.0f;

  presets_room[16].flGain = 1.0f;
  presets_room[16].flDecayTime = 3.5f;
  presets_room[16].flDecayHFRatio = 0.0f;

  presets_room[17].flGain = 0.65f;
  presets_room[17].flDecayTime = 1.493f;
  presets_room[17].flDecayHFRatio = 0.5f;

  presets_room[18].flGain = 0.85f;
  presets_room[18].flDecayTime = 1.493f;
  presets_room[18].flDecayHFRatio = 0.5f;

  presets_room[19].flGain = 1.0f;
  presets_room[19].flDecayTime = 1.493f;
  presets_room[19].flDecayHFRatio = 0.5f;

  presets_room[20].flGain = 0.4f;
  presets_room[20].flDecayTime = 7.284f;
  presets_room[20].flDecayHFRatio = 1.0f / 3.0f;

  presets_room[21].flGain = 0.55f;
  presets_room[21].flDecayTime = 7.284f;
  presets_room[21].flDecayHFRatio = 1.0f / 3.0f;

  presets_room[22].flGain = 0.7f;
  presets_room[22].flDecayTime = 7.284f;
  presets_room[22].flDecayHFRatio = 1.0f / 3.0f;

  presets_room[23].flGain = 0.5f;
  presets_room[23].flDecayTime = 3.961f;
  presets_room[23].flDecayHFRatio = 0.5f;

  presets_room[24].flGain = 0.7f;
  presets_room[24].flDecayTime = 3.961f;
  presets_room[24].flDecayHFRatio = 0.5f;

  presets_room[25].flGain = 1.0f;
  presets_room[25].flDecayTime = 3.961f;
  presets_room[25].flDecayHFRatio = 0.5f;

  presets_room[26].flGain = 0.2f;
  presets_room[26].flDecayTime = 17.234f;
  presets_room[26].flDecayHFRatio = 2.0f / 3.0f;

  presets_room[27].flGain = 0.3f;
  presets_room[27].flDecayTime = 17.234f;
  presets_room[27].flDecayHFRatio = 2.0f / 3.0f;

  presets_room[28].flGain = 0.4f;
  presets_room[28].flDecayTime = 17.234f;
  presets_room[28].flDecayHFRatio = 2.0f / 3.0f;
}

void EnvEffects::OverrideEffects()
{
  std::array<char, 256> directory;
  g_pInterface->FileSystem->GetCurrentDirectoryA(directory.data(), directory.size());
  std::filesystem::path filePath = directory.data();
  filePath.append("efx-reverb.json");

  EfxReader reader;
  auto reverbFromJson = reader.GetProperties(filePath.string());
  for (const auto& reverb : reverbFromJson)
  {
    auto index = std::get<0>(reverb);
    if (index < presets_room.size())
    {
      presets_room[index] = std::get<1>(reverb);
    }
  }
}

EnvEffects::~EnvEffects()
{
  for (auto& effectSlot : alAuxEffectSlots) {
    effectSlot.slot.destroy();
    effectSlot.effect.destroy();
  }
}