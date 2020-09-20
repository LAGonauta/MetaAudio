#include "Loaders/SoxrDecoder.hpp"
#include "Utilities/SoxrResamplerHelper.hpp"

namespace MetaAudio
{
  SoxrDecoder::SoxrDecoder(alure::String file_path, alure::Context context, size_t frequency)
  {
    auto& buffer = m_cache.find(file_path);
    if (buffer != m_cache.end())
    {
      m_buffer = buffer->second;
    }
    else
    {
      m_buffer = m_helper.GetAudio(context.createDecoder(file_path), frequency);
      m_cache.insert(std::make_pair(file_path, m_buffer));
    }
  }

  SoxrDecoder::SoxrDecoder(alure::SharedPtr<alure::Decoder> dec, size_t frequency)
  {
    m_buffer = m_helper.GetAudio(dec, frequency); // TODO: allow decoders with length == 0. We will need to resample the audio in parts.
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
    return m_buffer->hasLoopPoints;
  }

  std::pair<uint64_t, uint64_t> SoxrDecoder::getLoopPoints() const noexcept
  {
    return m_buffer->loopPoints;
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
    count_bytes = m_position + count_bytes > m_buffer->data.size() ?
      m_buffer->data.size() - m_position : count_bytes;

    memcpy_s(ptr, count_bytes, m_buffer->data.data() + m_position, count_bytes);
    m_position += count_bytes;
    return alure::BytesToFrames(count_bytes, m_buffer->channels, m_buffer->type);
  }
}
