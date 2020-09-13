#pragma once

#include "alure2.h"

namespace MetaAudio
{
  struct AudioBuffer
  {
    alure::Vector<ALubyte> data;
    alure::SampleType type;
    alure::ChannelConfig channels;
    size_t sample_rate;

    AudioBuffer(
      const alure::Vector<ALubyte>& data,
      alure::SampleType type,
      alure::ChannelConfig channels,
      size_t sample_rate)
      : data(data), type(type), channels(channels), sample_rate(sample_rate)
    { }
  };
}