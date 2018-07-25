#include <metahook.h>

#include "snd_local.h"
#include "snd_voice.hpp"

VoiceDecoder::VoiceDecoder(sfx_t *sound, aud_channel_t *ch)
{
  sfxcache_t *oldsc = (sfxcache_t *)gAudEngine.S_LoadSound(sound, nullptr);

  if (!oldsc)
    throw std::runtime_error("Unable to find voice cache.");

  m_ch = ch;

  VoiceSE_GetSoundDataCallback = (int(*)(sfxcache_s *, char *, int, int, int))oldsc->loopstart;

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
VoiceDecoder::~VoiceDecoder() {}

void VoiceDecoder::destroy()
{
  if (m_ch->voicecache)
  {
    m_ch->voicecache = nullptr;
    gAudEngine.VoiceSE_NotifyFreeChannel(m_ch->entchannel);
  }
}

ALuint VoiceDecoder::read(ALvoid *ptr, ALuint count) noexcept
{
	//invalid voice?
  if(!m_ch->voicecache)
    return 0;  

	size_t ulRecvedFrames = VoiceSE_GetSoundDataCallback(m_ch->voicecache, reinterpret_cast<char *>(ptr), alure::FramesToBytes(count, m_channel_config, m_sample_type), 0, count);

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
  return UINT64_MAX;
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