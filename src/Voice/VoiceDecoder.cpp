#include <metahook.h>

#include "Voice/VoiceDecoder.hpp"

namespace MetaAudio
{
  VoiceDecoder::VoiceDecoder(sfx_t* sound, aud_channel_t* ch)
  {
    sfxcache_t* oldsc = (sfxcache_t*)gAudEngine.S_LoadSound(sound, nullptr);

    if (!oldsc)
    {
      throw std::runtime_error("Unable to find voice cache.");
    }

    m_entchannel = ch->entchannel;

    if (m_entchannel < CHAN_NETWORKVOICE_BASE || m_entchannel > CHAN_NETWORKVOICE_END)
    {
      throw std::runtime_error("Bad entity channel for voice: " + std::to_string(m_entchannel) + ".");
    }

    VoiceSE_GetSoundDataCallback = (int(*)(sfxcache_s*, char*, int, int, int))oldsc->loopstart;

    m_voicecache = oldsc;
    ch->voicecache = oldsc;

    if (oldsc->stereo)
    {
      m_channel_config = alure::ChannelConfig::Stereo;
    }
    else
    {
      m_channel_config = alure::ChannelConfig::Mono;
    }

    if (oldsc->width == 2)
    {
      m_sample_type = alure::SampleType::Int16;
    }
    else
    {
      m_sample_type = alure::SampleType::UInt8;
    }

    m_sample_rate = oldsc->samplerate;
  }

  //not done
  VoiceDecoder::~VoiceDecoder()
  {
    gAudEngine.VoiceSE_NotifyFreeChannel(m_entchannel);
  }

  ALuint VoiceDecoder::read(ALvoid* ptr, ALuint count) noexcept
  {
    //invalid voice?
    if (!m_voicecache)
    {
      return 0;
    }

    size_t ulRecvedFrames = VoiceSE_GetSoundDataCallback(m_voicecache, static_cast<char*>(ptr), alure::FramesToBytes(count, m_channel_config, m_sample_type), 0, count);

    return ulRecvedFrames;
  }

  ALuint VoiceDecoder::getFrequency() const noexcept
  {
    return m_sample_rate;
  }

  alure::ChannelConfig VoiceDecoder::getChannelConfig() const noexcept
  {
    return m_channel_config;
  }

  alure::SampleType VoiceDecoder::getSampleType() const noexcept
  {
    return m_sample_type;
  }

  uint64_t VoiceDecoder::getLength() const noexcept
  {
    return 0;
  }

  bool VoiceDecoder::seek(uint64_t pos) noexcept
  {
    return false;
  }

  bool VoiceDecoder::hasLoopPoints() const noexcept
  {
    return false;
  }

  std::pair<uint64_t, uint64_t> VoiceDecoder::getLoopPoints() const noexcept
  {
    return std::pair<uint64_t, uint64_t>(0, UINT64_MAX);
  }
}