#pragma once

#include "BaseSoundSource.hpp"

namespace MetaAudio
{
  class SoundSourceFactory
  {
  public:
    static alure::SharedPtr<BaseSoundSource> GetStreamingSource(const alure::SharedPtr<alure::Decoder>& decoder, alure::Source& source, size_t chunk_len, size_t queue_len);
    static alure::SharedPtr<BaseSoundSource> GetStaticSource(const alure::Buffer& buffer, alure::Source& source);
    static alure::SharedPtr<BaseSoundSource> GetSteamAudioSource(const alure::SharedPtr<alure::Decoder>& decoder, alure::Source& source, size_t chunk_len, size_t queue_len);
    static alure::SharedPtr<BaseSoundSource> GetSteamAudioSource(const alure::String& file_path, alure::AutoObj<alure::Context>& context, alure::Source& source, size_t chunk_len, size_t queue_len);
  };
}