#include "SoundSources/SoundSourceFactory.hpp"
#include "SoundSources/StreamingSoundSource.hpp"
#include "SoundSources/StaticSoundSource.hpp"
#include "SoundSources/SteamAudioSoundSource.hpp"

namespace MetaAudio
{
  alure::SharedPtr<BaseSoundSource> SoundSourceFactory::GetStreamingSource(
    const alure::SharedPtr<alure::Decoder>& decoder,
    alure::Source& source,
    size_t chunk_len,
    size_t queue_len
    )
  {
    return alure::MakeShared<StreamingSoundSource>(decoder, source, chunk_len, queue_len);
  }

  alure::SharedPtr<BaseSoundSource> SoundSourceFactory::GetStaticSource(const alure::Buffer& buffer, alure::Source& source)
  {
    return alure::MakeShared<StaticSoundSource>(buffer, source);
  }

  alure::SharedPtr<BaseSoundSource> SoundSourceFactory::GetSteamAudioSource(const alure::SharedPtr<alure::Decoder>& decoder, alure::Source& source, size_t chunk_len, size_t queue_len)
  {
    return alure::SharedPtr<BaseSoundSource>();
  }

  alure::SharedPtr<BaseSoundSource> SoundSourceFactory::GetSteamAudioSource(const alure::String& file_path, alure::AutoObj<alure::Context>& context, alure::Source& source, size_t chunk_len, size_t queue_len)
  {
    return alure::SharedPtr<BaseSoundSource>();
  }
}