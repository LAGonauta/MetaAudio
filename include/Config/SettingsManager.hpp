#pragma once

#include "Enums/XFiWorkaround.hpp"
#include "Enums/OccluderType.hpp"
#include "Enums/ClampingMode.hpp"

namespace MetaAudio
{
  class SettingsManager
  {
  public:
    void Init();

    bool ResampleAll();

    ClampingMode OnResamplerClipping();

    bool NoSound();

    bool OcclusionEnabled();
    bool OcclusionFade();

    bool ReverbEnabled();
    size_t ReverbType();
    size_t ReverbUnderwaterType();

    bool SoundShow();

    float DopplerFactor();

    float Volume();

    OccluderType Occluder();

    XFiWorkaround XfiWorkaround();
  };
}

#ifndef _METAAUDIO_SETTINGS_MANAGER_
#define _METAAUDIO_SETTINGS_MANAGER_
extern MetaAudio::SettingsManager settings;
#endif
