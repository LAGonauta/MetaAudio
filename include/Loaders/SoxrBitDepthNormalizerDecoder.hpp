#pragma once

#include "alure2.h"
#include "Structures/AudioBuffer.hpp"
#include "Utilities/SoxrResamplerHelper.hpp"

namespace MetaAudio
{
  class SoxrBitDepthNormalizerDecoder final : public alure::Decoder
  {
  private:
    alure::Vector<ALubyte> m_scratch_buffer;
    alure::SharedPtr<alure::Decoder> m_decoder = nullptr;
    std::unique_ptr<std::remove_pointer_t<soxr_t>, decltype(&soxr_delete)> m_soxr;
    SoxrResamplerHelper m_helper;

  public:
    SoxrBitDepthNormalizerDecoder(alure::SharedPtr<alure::Decoder> dec);

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