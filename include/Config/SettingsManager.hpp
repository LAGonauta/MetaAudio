#pragma once

#include <metahook.h>

#include "Enums/XFiWorkaround.hpp"
#include "Enums/OccluderType.hpp"
#include "Enums/ClampingMode.hpp"

namespace MetaAudio
{
    class SettingsManager final
    {
    public:
        void Init(const cl_enginefunc_t& engFuncs);

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

    const int SOXR_SAMPLE_RATE = 48000;
}

#ifndef _METAAUDIO_SETTINGS_MANAGER_
#define _METAAUDIO_SETTINGS_MANAGER_
extern MetaAudio::SettingsManager settings;
#endif