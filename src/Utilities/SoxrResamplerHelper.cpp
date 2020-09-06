#include "Utilities/SoxrResamplerHelper.hpp"

namespace MetaAudio
{
  SoxrResamplerHelper::SoxrResamplerHelper() : runtimeSpec(soxr_runtime_spec(0)), qualitySpec(soxr_quality_spec(SOXR_VHQ, 0))
  {
  }

  std::shared_ptr<AudioBuffer> SoxrResamplerHelper::GetAudio(std::string filename, alure::Context al_context, size_t finalSampleRate)
  {
    auto dec = al_context.createDecoder(filename);
    if (dec->getFrequency() == finalSampleRate && dec->getSampleType() == alure::SampleType::Float32)
    {
      std::vector<float> audioData(dec->getLength() * GetChannelQuantity(dec->getChannelConfig()));
      dec->read(audioData.data(), dec->getLength());
      return std::make_shared<AudioBuffer>(audioData, alure::SampleType::Float32, dec->getChannelConfig());
    }

    std::vector<ALbyte> audioData(alure::FramesToBytes(dec->getLength(), dec->getChannelConfig(), dec->getSampleType()));
    dec->read(audioData.data(), dec->getLength());

    auto inputSampleType = dec->getSampleType();
    auto inputLength = dec->getLength();
    auto inputSize = audioData.size();
    auto inputFrequency = dec->getFrequency();
    auto inputChannelConfig = dec->getChannelConfig();

    std::vector<float> resampledAudio(inputSize / GetSampleSize(inputSampleType) * finalSampleRate / inputFrequency + 1);

    auto audioSpec = soxr_io_spec(GetSoxType(inputSampleType), SOXR_FLOAT32_I);
    auto resampler = soxr_create(inputFrequency, finalSampleRate, GetChannelQuantity(inputChannelConfig), nullptr, &audioSpec, &qualitySpec, &runtimeSpec);

    size_t j = 0;
    for (size_t i = 0,
      block = 262144,
      idone = 0,
      odone = 0,
      m = alure::BytesToFrames(audioData.size(), inputChannelConfig, inputSampleType);
      i < m;)
    {
      if (block + i > m)
      {
        block = m - i;
      }
      soxr_process(resampler, audioData.data() + alure::FramesToBytes(i, inputChannelConfig, inputSampleType),
        block, &idone,
        resampledAudio.data() + j, resampledAudio.size() - j, &odone);

      i += idone;
      j += odone * GetChannelQuantity(inputChannelConfig);
      if (odone == 0)
      {
        break;
      }
    }
    resampledAudio.shrink_to_fit();

    soxr_delete(resampler);
    return std::make_shared<AudioBuffer>(resampledAudio, alure::SampleType::Float32, inputChannelConfig);
  }

  size_t SoxrResamplerHelper::GetSampleSize(alure::SampleType type)
  {
    switch (type)
    {
    case alure::SampleType::Float32:
      return 4;
    case alure::SampleType::Int16:
      return 2;
    case alure::SampleType::Mulaw:
    case alure::SampleType::UInt8:
      return 1;
    }
  }

  soxr_datatype_t SoxrResamplerHelper::GetSoxType(alure::SampleType type)
  {
    switch (type)
    {
    case alure::SampleType::Float32:
      return SOXR_FLOAT32_I;
    case alure::SampleType::Int16:
      return SOXR_INT16_I;
    default:
      throw std::exception("Sample type not supported");
    }
  }

  size_t SoxrResamplerHelper::GetChannelQuantity(alure::ChannelConfig config)
  {
    switch (config)
    {
    case alure::ChannelConfig::Mono:
      return 1;
    case alure::ChannelConfig::Stereo:
      return 2;
    default:
      throw std::exception("Channel type not implemented.");
    }
  }
}