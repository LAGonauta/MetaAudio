#pragma once

#include "BaseSoundSource.hpp"

namespace MetaAudio
{
  class StreamingSoundSource final : public BaseSoundSource
  {
  private:
    alure::SharedPtr<alure::Decoder> m_decoder;
    size_t chunk_len;
    size_t queue_len;

  public:
    StreamingSoundSource(const alure::SharedPtr<alure::Decoder>& decoder, alure::Source& source, size_t chunk_len, size_t queue_len);

    void Play() override;
  };
}