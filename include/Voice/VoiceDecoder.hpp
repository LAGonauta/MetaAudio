#pragma once

#include "snd_local.h"

namespace MetaAudio
{
  class VoiceDecoder final : public alure::Decoder
  {
    int(*VoiceSE_GetSoundDataCallback)(sfxcache_s* cache, char* copyBuf, int maxOutDataSize, int samplePos, int sampleCount);

    alure::ChannelConfig m_channel_config{ alure::ChannelConfig::Mono };
    alure::SampleType m_sample_type{ alure::SampleType::UInt8 };
    size_t m_sample_rate{ 8000 };

    aud_channel_t* m_ch;

  public:
    VoiceDecoder(sfx_t* sound, aud_channel_t* ch);
    ~VoiceDecoder() override;

    void destroy();

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