#include "SoundSources/StreamingSoundSource.hpp"

namespace MetaAudio
{
  StreamingSoundSource::StreamingSoundSource(const alure::SharedPtr<alure::Decoder>& decoder, alure::Source& source, size_t chunk_len, size_t queue_len)
    : m_decoder(decoder), chunk_len(chunk_len), queue_len(queue_len)
  {
    m_source = alure::MakeAuto<alure::Source>(source);
  }

  void StreamingSoundSource::Play()
  {
    m_source->play(m_decoder, chunk_len, queue_len);
  }
}