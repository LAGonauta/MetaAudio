#pragma once

#include "alure2.h"

namespace MetaAudio
{
  struct SteamAudioBuffer
  {
    std::vector<float> data;
    alure::SampleType type;
    alure::ChannelConfig channels;

    SteamAudioBuffer(const std::vector<float>& data, alure::SampleType type, alure::ChannelConfig channels) : data(data), type(type), channels(channels) { }
  };
}