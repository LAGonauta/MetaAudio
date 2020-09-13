#include "snd_local.h"
#include "Config/SettingsManager.hpp"

namespace MetaAudio
{
  //engine cvars
  static cvar_t* nosound = nullptr;
  static cvar_t* snd_show = nullptr;
  static cvar_t* sxroom_off = nullptr;
  static cvar_t* sxroom_type = nullptr;
  static cvar_t* sxroomwater_type = nullptr;
  static cvar_t* volume = nullptr;

  //active control
  static cvar_t* al_doppler = nullptr;
  static cvar_t* al_occluder = nullptr;
  static cvar_t* al_occlusion = nullptr;
  static cvar_t* al_occlusion_fade = nullptr;
  static cvar_t* al_xfi_workaround = nullptr;

  static constexpr char* DEFAULT_XFI_WORKAROUND = "0"; // Disabled
  static constexpr char* DEFAULT_OCCLUDER = "0"; // GoldSrc
  static constexpr char* DEFAULT_OCCLUSION = "1";
  static constexpr char* DEFAULT_OCCLUSION_FADE = "1";
  static constexpr char* DEFAULT_DOPPLER_FACTOR = "0.3";

  void SettingsManager::Init()
  {
    if (al_xfi_workaround == nullptr) al_xfi_workaround = gEngfuncs.pfnRegisterVariable("al_xfi_workaround", DEFAULT_XFI_WORKAROUND, FCVAR_EXTDLL);
    if (al_doppler == nullptr) al_doppler = gEngfuncs.pfnRegisterVariable("al_doppler", DEFAULT_DOPPLER_FACTOR, FCVAR_EXTDLL);
    if (al_occluder == nullptr) al_occluder = gEngfuncs.pfnRegisterVariable("al_occluder", DEFAULT_OCCLUDER, FCVAR_EXTDLL);
    if (al_occlusion == nullptr) al_occlusion = gEngfuncs.pfnRegisterVariable("al_occlusion", DEFAULT_OCCLUSION, FCVAR_EXTDLL);
    if (al_occlusion_fade == nullptr) al_occlusion_fade = gEngfuncs.pfnRegisterVariable("al_occlusion_fade", DEFAULT_OCCLUSION_FADE, FCVAR_EXTDLL);

    if (nosound == nullptr) nosound = gEngfuncs.pfnGetCvarPointer("nosound");
    if (volume == nullptr) volume = gEngfuncs.pfnGetCvarPointer("volume");
    if (sxroomwater_type == nullptr) sxroomwater_type = gEngfuncs.pfnGetCvarPointer("waterroom_type");
    if (sxroom_type == nullptr) sxroom_type = gEngfuncs.pfnGetCvarPointer("room_type");
    if (snd_show == nullptr) snd_show = gEngfuncs.pfnGetCvarPointer("snd_show");
  }

  bool SettingsManager::NoSound()
  {
    return nosound == nullptr || static_cast<bool>(nosound->value);
  }

  bool SettingsManager::SoundShow()
  {
    return static_cast<bool>(snd_show->value);
  }

  float SettingsManager::DopplerFactor()
  {
    return al_doppler->value;
  }

  float SettingsManager::Volume()
  {
    return volume->value;
  }

  bool SettingsManager::ReverbEnabled()
  {
    return !static_cast<bool>(sxroom_off->value);
  }

  size_t SettingsManager::ReverbType()
  {
    return static_cast<size_t>(sxroom_type->value);
  }

  size_t SettingsManager::ReverbUnderwaterType()
  {
    return static_cast<size_t>(sxroomwater_type->value);
  }

  XFiWorkaround SettingsManager::XfiWorkaround()
  {
    return static_cast<XFiWorkaround>(al_xfi_workaround->value);
  }
  OccluderType SettingsManager::Occluder()
  {
    return static_cast<OccluderType>(al_occluder->value);
  }
  bool SettingsManager::OcclusionEnabled()
  {
    return static_cast<bool>(al_occlusion->value);
  }
  bool SettingsManager::OcclusionFade()
  {
    return static_cast<bool>(al_occlusion_fade->value);
  }
}