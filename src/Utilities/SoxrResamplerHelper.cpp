#include <fstream>

#include "Utilities/SoxrResamplerHelper.hpp"

namespace MetaAudio
{
  SoxrResamplerHelper::SoxrResamplerHelper()
    : runtimeSpec(soxr_runtime_spec(0)), qualitySpec(soxr_quality_spec(SOXR_VHQ, 0))
  {
  }

  std::shared_ptr<AudioBuffer> SoxrResamplerHelper::GetAudio(alure::SharedPtr<alure::Decoder> dec, size_t finalSampleRate)
  {
    alure::Vector<ALubyte> audioData(alure::FramesToBytes(dec->getLength(), dec->getChannelConfig(), dec->getSampleType()));
    dec->read(audioData.data(), dec->getLength());

    if (dec->getFrequency() == finalSampleRate && dec->getSampleType() == alure::SampleType::Float32)
    {
      return alure::MakeShared<AudioBuffer>(
        audioData,
        alure::SampleType::Float32,
        dec->getChannelConfig(),
        dec->getFrequency(),
        dec->hasLoopPoints(),
        dec->getLoopPoints()
        );
    }

    auto inputSampleType = dec->getSampleType();
    auto inputLength = dec->getLength();
    auto inputSize = audioData.size();
    auto inputFrequency = dec->getFrequency();
    auto inputChannelConfig = dec->getChannelConfig();

    if (inputSampleType == alure::SampleType::UInt8)
    {
      alure::Vector<int16_t> int_data(audioData.size());
      for (size_t i = 0; i < int_data.size(); ++i)
      {
        int_data[i] = static_cast<int16_t>(audioData[i] - 128) << 8;
      }
      
      auto reinterpreted = alure::ArrayView<int16_t>(int_data).reinterpret_as<ALubyte>();
      audioData = alure::Vector<ALubyte>(reinterpreted.begin(), reinterpreted.end());

      inputSampleType = alure::SampleType::Int16;
    }

    std::vector<float> resampledAudio(dec->getLength() * GetChannelQuantity(dec->getChannelConfig()) * finalSampleRate / inputFrequency + .5);

    auto audioSpec = soxr_io_spec(GetSoxType(inputSampleType), SOXR_FLOAT32_I);

    size_t odone, idone;
    auto err = soxr_oneshot(
      inputFrequency, finalSampleRate, GetChannelQuantity(inputChannelConfig),
      audioData.data(), dec->getLength(), &idone,
      resampledAudio.data(), resampledAudio.size() / GetChannelQuantity(inputChannelConfig), &odone,
      &audioSpec, &qualitySpec, &runtimeSpec
    );

    if (odone > 0)
    {
      resampledAudio.resize(odone);

      auto max = std::abs(*std::max_element(
        resampledAudio.cbegin(),
        resampledAudio.cend(),
        [](const auto& lhs, const auto& rhs) { return std::abs(lhs) < std::abs(rhs); }
      ));
      if (max > 1.0f)
      {
        std::for_each(resampledAudio.begin(), resampledAudio.end(), [=](auto& value) { value /= max; });
      }
    }

    auto array_view = alure::ArrayView<float>(resampledAudio).reinterpret_as<ALubyte>();

    auto loopPoints = dec->getLoopPoints();
    if (dec->hasLoopPoints())
    {
      auto ratio = alure::BytesToFrames(array_view.size(), inputChannelConfig, alure::SampleType::Float32) / dec->getLength();
      loopPoints.first *= ratio;
      loopPoints.second *= ratio;
    }

    return alure::MakeShared<AudioBuffer>(
      alure::Vector<ALubyte>(array_view.begin(), array_view.end()),
      alure::SampleType::Float32,
      inputChannelConfig,
      finalSampleRate,
      dec->hasLoopPoints(),
      loopPoints
      );
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

    throw std::runtime_error("Sample type not supported: " + std::to_string(static_cast<size_t>(type)));
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