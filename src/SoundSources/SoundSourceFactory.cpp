#include "SoundSources/SoundSourceFactory.hpp"
#include "SoundSources/StreamingSoundSource.hpp"
#include "SoundSources/StaticSoundSource.hpp"

namespace MetaAudio
{
  std::shared_ptr<BaseSoundSource> SoundSourceFactory::GetStreamingSource(
    const alure::SharedPtr<alure::Decoder>& decoder,
    alure::Source& source,
    size_t chunk_len,
    size_t queue_len
    )
  {
    return std::make_shared<StreamingSoundSource>(decoder, source, chunk_len, queue_len);
  }

  std::shared_ptr<BaseSoundSource> SoundSourceFactory::GetStaticSource(const alure::Buffer& buffer, alure::Source& source)
  {
    return std::make_shared<StaticSoundSource>(buffer, source);
  }
}