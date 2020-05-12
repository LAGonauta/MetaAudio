#pragma once

#include "BaseSoundSource.hpp"

namespace MetaAudio
{
  class SoundSourceFactory
  {
  public:
    static std::shared_ptr<BaseSoundSource> GetStreamingSource(const alure::SharedPtr<alure::Decoder>& decoder, alure::Source& source, size_t chunk_len, size_t queue_len);
    static std::shared_ptr<BaseSoundSource> GetStaticSource(const alure::Buffer& buffer, alure::Source& source);
    static std::shared_ptr<BaseSoundSource> GetSteamAudioSource();
  };
}