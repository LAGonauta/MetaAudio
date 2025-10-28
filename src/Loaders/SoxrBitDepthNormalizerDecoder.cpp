#include "Loaders/SoxrBitDepthNormalizerDecoder.hpp"

namespace MetaAudio
{
	SoxrBitDepthNormalizerDecoder::SoxrBitDepthNormalizerDecoder(alure::SharedPtr<alure::Decoder> dec) : m_decoder(dec), m_soxr(nullptr, &soxr_delete)
  {
    soxr_error_t error;
    auto runtime_spec = soxr_runtime_spec(0);
    auto quality = soxr_quality_spec(SOXR_QQ, 0);
    auto io_spec = soxr_io_spec(m_helper.GetSoxType(dec->getSampleType()), SOXR_INT16_I);

    m_soxr = std::unique_ptr<std::remove_pointer_t<soxr_t>, decltype(&soxr_delete)>(
        soxr_create(
            dec->getFrequency(),
            dec->getFrequency(),
            m_helper.GetChannelQuantity(dec->getChannelConfig()),
            &error,
            &io_spec,
            &quality,
            &runtime_spec
        ),
        &soxr_delete
    );

    if (error)
    {
      throw std::runtime_error("Unable to create soxr instance");
    }
  }

  ALuint SoxrBitDepthNormalizerDecoder::getFrequency() const noexcept
  {
    return m_decoder->getFrequency();
  }

  alure::ChannelConfig SoxrBitDepthNormalizerDecoder::getChannelConfig() const noexcept
  {
    return m_decoder->getChannelConfig();
  }

  alure::SampleType SoxrBitDepthNormalizerDecoder::getSampleType() const noexcept
  {
    return alure::SampleType::Int16;
  }
  bool SoxrBitDepthNormalizerDecoder::hasLoopPoints() const noexcept
  {
    return m_decoder->hasLoopPoints();
  }

  std::pair<uint64_t, uint64_t> SoxrBitDepthNormalizerDecoder::getLoopPoints() const noexcept
  {
    return m_decoder->getLoopPoints();
  }

  uint64_t SoxrBitDepthNormalizerDecoder::getLength() const noexcept
  {
    return m_decoder->getLength();
  }

  bool SoxrBitDepthNormalizerDecoder::seek(uint64_t pos) noexcept
  {
    if (m_decoder->seek(pos))
    {
      soxr_clear(m_soxr.get());
      return true;
    }

    return false;
  }
  ALuint SoxrBitDepthNormalizerDecoder::read(ALvoid* ptr, ALuint count) noexcept
  {
    if (m_decoder->getSampleType() == alure::SampleType::Int16)
    {
      return m_decoder->read(ptr, count);
    }

    // Prepare a buffer large enough for 'count' frames of the decoder's sample type
	m_scratch_buffer.resize(alure::FramesToBytes(count, m_decoder->getChannelConfig(), m_decoder->getSampleType()));
    // Read up to 'count' frames from upstream decoder
    auto read = m_decoder->read(m_scratch_buffer.data(), count);

    size_t idone = 0;
    size_t odone = 0;
    soxr_error_t error = nullptr;

    if (read == 0)
    {
      // No more input available; flush any remaining samples from soxr
      error = soxr_process(m_soxr.get(), nullptr, 0, nullptr, ptr, count, &odone);
    }
    else
    {
      // Provide the actual number of frames read as input
      error = soxr_process(m_soxr.get(), m_scratch_buffer.data(), static_cast<size_t>(read), &idone, ptr, static_cast<size_t>(count), &odone);
    }

    if (error)
    {
      // On soxr error, return 0 frames produced
      return 0;
    }

    // Consume any clipping information if needed (currently ignored)
    //auto clipped = soxr_num_clips(m_soxr.get());

    return static_cast<ALuint>(odone);
  }
}
