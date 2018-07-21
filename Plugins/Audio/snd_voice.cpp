#include <metahook.h>
#include "exportfuncs.h"
#include "FileSystem.h"
#include "util.h"
#include "snd_local.h"
#include "zone.h"

#define VOICE_BUFFER_SIZE 4096

VoiceDecoder::VoiceDecoder(sfx_t *sound, aud_channel_t *ch)
{
  sfxcache_t *oldsc = (sfxcache_t *)gAudEngine.S_LoadSound(sound, nullptr);

  if (!oldsc)
    throw std::runtime_error("Unable to find voice cache.");

  //m_ch = ch;
  m_entchannel = ch->entchannel;

  VoiceSE_GetSoundDataCallback = (int(*)(sfxcache_s *, char *, int, int, int))oldsc->loopstart;

  //ch->voicecache = oldsc;
  m_sfxcache_t = oldsc;

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
	//m_ch->voicecache = nullptr;
	gAudEngine.VoiceSE_NotifyFreeChannel(m_entchannel);
}

ALuint VoiceDecoder::read(ALvoid *ptr, ALuint count) noexcept
{
	//invalid voice?
	if(!m_sfxcache_t)
		return false;

  size_t width = 1;
  if (m_sample_type == alure::SampleType::Int16)
  {
    width = 2;
  }
  size_t channels = 1;
  if (m_channel_config == alure::ChannelConfig::Stereo)
  {
    channels = 2;
  }

	size_t ulRecvedFrames = VoiceSE_GetSoundDataCallback(m_sfxcache_t, reinterpret_cast<char *>(ptr), count * width * channels, 0, count);

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