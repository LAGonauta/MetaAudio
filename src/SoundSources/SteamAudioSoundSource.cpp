#include "SoundSources/SteamAudioSoundSource.hpp"

namespace MetaAudio
{
  SteamAudioSoundSource::SteamAudioSoundSource(const alure::SharedPtr<SteamAudioSoundDecoder>& decoder, alure::Source& source, size_t chunk_len, size_t queue_len)
    : m_decoder(decoder), chunk_len(chunk_len), queue_len(queue_len)
  {
    m_source = alure::MakeAuto<alure::Source>(source);
    m_source->set3DSpatialize(alure::Spatialize::Off);
  }

  void SteamAudioSoundSource::Play()
  {
    m_source->play(m_decoder, chunk_len, queue_len);
  }

  void SteamAudioSoundSource::SetPosition(alure::Vector3 position)
  {
    m_decoder->SetPosition(position);
  }

  void SteamAudioSoundSource::SetVelocity(alure::Vector3 velocity)
  {
    m_source->setVelocity(velocity);
  }

  void SteamAudioSoundSource::SetRadius(float radius)
  {
    m_decoder->SetSourceRadius(radius);
  }

  void SteamAudioSoundSource::SetLooping(bool value)
  {
    m_decoder->SetLooping(value);
  }

  void SteamAudioSoundSource::SetListener(alure::Vector3 pos, alure::Vector3 ahead, alure::Vector3 up)
  {
    m_decoder->SetListener(pos, ahead, up);
  }

  void SteamAudioSoundSource::SetRelative(bool value)
  {
    m_decoder->SetRelative(value);
    m_source->setRelative(value);
  }
}