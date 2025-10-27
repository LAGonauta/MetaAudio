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
    bool hasLoopPoints;
    std::pair<uint64_t, uint64_t> loopPoints;

    AudioBuffer(
      const alure::Vector<ALubyte>& data,
      alure::SampleType type,
      alure::ChannelConfig channels,
      size_t sample_rate,
      bool hasLoopPoints,
      std::pair<uint64_t, uint64_t> loopPoints
      )
      : data(data), type(type), channels(channels), sample_rate(sample_rate),
      hasLoopPoints(hasLoopPoints), loopPoints(loopPoints)
    { }
  };
}