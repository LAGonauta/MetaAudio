#pragma once

#include "alure2.h"
#include "Structures/AudioBuffer.hpp"
#include "Utilities/SoxrResamplerHelper.hpp"

namespace MetaAudio
{
  class SoxrBitDepthNormalizerDecoder final : public alure::Decoder
  {
  private:
    alure::SharedPtr<alure::Decoder> m_decoder = nullptr;
    soxr_t m_soxr = nullptr;
    SoxrResamplerHelper m_helper;

  public:
    SoxrBitDepthNormalizerDecoder(alure::SharedPtr<alure::Decoder> dec);
    ~SoxrBitDepthNormalizerDecoder();

    ALuint getFrequency() const noexcept override;
    alure::ChannelConfig getChannelConfig() const noexcept override;
    alure::SampleType getSampleType() const noexcept override;

    bool hasLoopPoints() const noexcept override;
    std::pair<uint64_t, uint64_t> getLoopPoints() const noexcept override;

    uint64_t getLength() const noexcept override;
    bool seek(uint64_t pos) noexcept override;
    ALuint read(ALvoid* ptr, ALuint count) noexcept override;
  };
}