#pragma once

#include "BaseSoundSource.hpp"
#include "../Loaders/SteamAudioSoundDecoder.hpp"

namespace MetaAudio
{
  class SteamAudioSoundSource : public BaseSoundSource
  {
  private:
    alure::SharedPtr<SteamAudioSoundDecoder> m_decoder;
    size_t chunk_len;
    size_t queue_len;

  public:
    SteamAudioSoundSource(const alure::SharedPtr<SteamAudioSoundDecoder>& sa_decoder, alure::Source& source, size_t chunk_len, size_t queue_len);

    void Play();

    void SetPosition(alure::Vector3 position);
    void SetVelocity(alure::Vector3 velocity);
    void SetRadius(float radius);
    void SetLooping(bool value);

    void SetListener(alure::Vector3 pos, alure::Vector3 ahead, alure::Vector3 up);
    void SetRelative(bool value);
  };
}