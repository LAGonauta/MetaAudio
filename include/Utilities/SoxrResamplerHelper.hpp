#pragma once

#include "alure2.h"
#include "soxr.h"

#include "Structures/AudioBuffer.hpp"

namespace MetaAudio
{
  class SoxrResamplerHelper final
  {
  private:
    soxr_runtime_spec_t runtimeSpec;
    soxr_quality_spec_t qualitySpec;

    size_t GetSampleSize(alure::SampleType type);
    soxr_datatype_t GetSoxType(alure::SampleType type);
    size_t GetChannelQuantity(alure::ChannelConfig config);
  public:
    SoxrResamplerHelper();

    std::shared_ptr<AudioBuffer> GetAudio(alure::SharedPtr<alure::Decoder> dec, size_t finalSampleRate);
  };
}