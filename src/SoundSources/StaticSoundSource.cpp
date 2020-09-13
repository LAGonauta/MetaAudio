#include "snd_local.h"

#include "SoundSources/StaticSoundSource.hpp"
#include "Config/SettingsManager.hpp"

namespace MetaAudio
{
  StaticSoundSource::StaticSoundSource(const alure::Buffer& buffer, alure::Source& source): m_buffer(buffer)
  {
    m_source = alure::AutoObj(source);
    m_frequency = buffer.getFrequency();
    m_length = buffer.getLength();
  }

  void StaticSoundSource::Play()
  {
    m_source->play(m_buffer);
    m_start_time = std::chrono::steady_clock::now();
    m_maximum_end_time = m_start_time + std::chrono::milliseconds(static_cast<long long>(static_cast<double>(m_length) / m_frequency * 1.5 * 1000)); // 50% of safety
  }

  bool StaticSoundSource::IsPlaying()
  {
    if (settings.XfiWorkaround() == XFiWorkaround::Timer)
    {
      return m_source->isPlaying() && std::chrono::steady_clock::now() < m_maximum_end_time;
    }
    else
    {
      return m_source->isPlaying();
    }
  }
}