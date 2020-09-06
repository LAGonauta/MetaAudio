#pragma once

#include "alure2.h"

namespace MetaAudio
{
  struct AudioBuffer
  {
    std::vector<float> data;
    alure::SampleType type;
    alure::ChannelConfig channels;

    AudioBuffer(const std::vector<float>& data, alure::SampleType type, alure::ChannelConfig channels) : data(data), type(type), channels(channels) { }
  };
}