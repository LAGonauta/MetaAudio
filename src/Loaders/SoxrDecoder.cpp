#include "Loaders/SoxrDecoder.hpp"
#include "Utilities/SoxrResamplerHelper.hpp"

namespace MetaAudio
{
  SoxrDecoder::SoxrDecoder(alure::SharedPtr<alure::Decoder> dec)
  {
    m_buffer = m_helper.GetAudio(dec, 48000);
  }

  ALuint SoxrDecoder::getFrequency() const noexcept
  {
    return m_buffer->sample_rate;
  }

  alure::ChannelConfig SoxrDecoder::getChannelConfig() const noexcept
  {
    return m_buffer->channels;
  }

  alure::SampleType SoxrDecoder::getSampleType() const noexcept
  {
    return m_buffer->type;
  }

  bool SoxrDecoder::hasLoopPoints() const noexcept
  {
    return false;
  }

  std::pair<uint64_t, uint64_t> SoxrDecoder::getLoopPoints() const noexcept
  {
    return { 0, std::numeric_limits<uint64_t>::max() };
  }

  uint64_t SoxrDecoder::getLength() const noexcept
  {
    return alure::BytesToFrames(m_buffer->data.size(), m_buffer->channels, m_buffer->type);
  }

  bool SoxrDecoder::seek(uint64_t pos) noexcept
  {
    auto bytes = alure::FramesToBytes(pos, m_buffer->channels, m_buffer->type);
    if (bytes < m_buffer->data.size())
    {
      m_position = alure::FramesToBytes(pos, m_buffer->channels, m_buffer->type);
      return true;
    }
    
    return false;
  }

  ALuint SoxrDecoder::read(ALvoid* ptr, ALuint count) noexcept
  {
    auto count_bytes = alure::FramesToBytes(count, m_buffer->channels, m_buffer->type);
    auto new_pos = m_position + count_bytes;
    if (new_pos > m_buffer->data.size())
    {
      new_pos = m_buffer->data.size() - 1;
    }
    
    auto ret = new_pos - m_position;
    memcpy_s(ptr, count_bytes, m_buffer->data.data() + m_position, ret);
    m_position = new_pos;
    return alure::BytesToFrames(ret, m_buffer->channels, m_buffer->type);
  }
}
