#include "Loaders/SoxrBitDepthNormalizerDecoder.hpp"

namespace MetaAudio
{
  SoxrBitDepthNormalizerDecoder::SoxrBitDepthNormalizerDecoder(alure::SharedPtr<alure::Decoder> dec) : m_decoder(dec)
  {
    SoxrResamplerHelper helper;
    soxr_error_t error;
    auto runtime_spec = soxr_runtime_spec(0);
    auto quality = soxr_quality_spec(SOXR_QQ, 0);
    auto io_spec = soxr_io_spec(helper.GetSoxType(dec->getSampleType()), SOXR_INT16_I);

    m_soxr = soxr_create(dec->getFrequency(), dec->getFrequency(), helper.GetChannelQuantity(dec->getChannelConfig()), &error, &io_spec, &quality, &runtime_spec);

    if (error)
    {
      throw std::runtime_error("Unable to create soxr instance");
    }
  }

  SoxrBitDepthNormalizerDecoder::~SoxrBitDepthNormalizerDecoder()
  {
    if (m_soxr)
    {
      soxr_delete(m_soxr);
      m_soxr = nullptr;
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
      soxr_clear(m_soxr);
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

    alure::Vector<ALubyte> read_data(alure::FramesToBytes(count, m_decoder->getChannelConfig(), m_decoder->getSampleType()));
    auto read = m_decoder->read(read_data.data(), count);

    size_t idone = 0;
    size_t odone = 0;
    // ~ on input count means flush
    auto error = soxr_process(m_soxr, read_data.data(), ~count, &idone, ptr, count, &odone);

    auto clipped = soxr_num_clips(m_soxr);

    return odone;
  }
}