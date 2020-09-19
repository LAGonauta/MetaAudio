#pragma once

#include "alure2.h"
#include "Structures/AudioBuffer.hpp"
#include "Utilities/SoxrResamplerHelper.hpp"

namespace MetaAudio
{
  class SoxrDecoder final : public alure::Decoder
  {
  private:
    alure::SharedPtr<AudioBuffer> m_buffer = nullptr;
    size_t m_position = 0;
    SoxrResamplerHelper m_helper;

  public:
    SoxrDecoder(alure::SharedPtr<alure::Decoder> dec, size_t frequency = 48000);

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