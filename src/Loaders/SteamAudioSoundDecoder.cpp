#include "Loaders/SteamAudioSoundDecoder.hpp"
#include "Loaders/SoxrDecoder.hpp"
#include "Loaders/SoxrBitDepthNormalizerDecoder.hpp"

namespace MetaAudio
{
  SteamAudioSoundDecoder::SteamAudioSoundDecoder(
    alure::SharedPtr<SteamAudio> sa,
    alure::SharedPtr<SteamAudioMapMeshLoader> mesh_loader,
    alure::SharedPtr<alure::Decoder> decoder,
    alure::SharedPtr<IPLhandle> binaural_renderer,
    alure::SharedPtr<IPLhandle> env_renderer,
    size_t frame_size)
    : m_steamaudio(sa), m_mesh_loader(mesh_loader), m_decoder(decoder), m_binaural_renderer(binaural_renderer), m_env_renderer(env_renderer), m_frame_size(frame_size)
  {
    if (m_decoder->getFrequency() != FREQUENCY)
    {
      m_decoder = alure::MakeShared<SoxrDecoder>(m_decoder, FREQUENCY);
    }

    m_input.numSpeakers = GetChannelQuantity(m_decoder->getChannelConfig());
    m_input.channelLayout = GetChannelLayout(m_decoder->getChannelConfig());
    m_input.channelOrder = IPLChannelOrder::IPL_CHANNELORDER_INTERLEAVED;
    m_input.channelLayoutType = GetChannelLayoutType(m_decoder->getChannelConfig());

    m_output.numSpeakers = GetChannelQuantity(OUTPUT_CHANNEL_CONFIG);
    m_output.channelLayout = GetChannelLayout(OUTPUT_CHANNEL_CONFIG);
    m_output.channelOrder = IPLChannelOrder::IPL_CHANNELORDER_INTERLEAVED;
    m_output.channelLayoutType = GetChannelLayoutType(OUTPUT_CHANNEL_CONFIG);

    m_intermediate_output.numSpeakers = GetChannelQuantity(OUTPUT_CHANNEL_CONFIG);
    m_intermediate_output.channelLayout = GetChannelLayout(OUTPUT_CHANNEL_CONFIG);
    m_intermediate_output.channelOrder = IPLChannelOrder::IPL_CHANNELORDER_DEINTERLEAVED;
    m_intermediate_output.channelLayoutType = GetChannelLayoutType(OUTPUT_CHANNEL_CONFIG);

    IPLRenderingSettings settings;
    settings.frameSize = m_frame_size;
    settings.samplingRate = FREQUENCY;
    settings.convolutionType = IPLConvolutionType::IPL_CONVOLUTIONTYPE_PHONON;

    {
        auto effect = new IPLhandle;
        auto error = m_steamaudio->iplCreateDirectSoundEffect(m_input, m_output, settings, effect);
        if (error)
        {
          delete effect;
          throw std::runtime_error("Unable to create direct effect. Error: " + std::to_string(error));
        }
        m_direct_effect = alure::SharedPtr<IPLhandle>(effect, [=](IPLhandle* effect) { m_steamaudio->iplDestroyDirectSoundEffect(effect); delete effect; });
    }

    //auto context = alure::Context::GetCurrent();
    //if (!context.isSupported(m_decoder->getChannelConfig(), m_decoder->getSampleType()))
    //{
    //  m_decoder = alure::MakeShared<SoxrBitDepthNormalizerDecoder>(m_decoder);
    //}
  }

  ALuint SteamAudioSoundDecoder::getFrequency() const noexcept
  {
    return FREQUENCY;
  }

  alure::ChannelConfig SteamAudioSoundDecoder::getChannelConfig() const noexcept
  {
    return OUTPUT_CHANNEL_CONFIG;
  }

  alure::SampleType SteamAudioSoundDecoder::getSampleType() const noexcept
  {
    return alure::SampleType::Float32;
  }

  bool SteamAudioSoundDecoder::hasLoopPoints() const noexcept
  {
    return false;
  }

  std::pair<uint64_t, uint64_t> SteamAudioSoundDecoder::getLoopPoints() const noexcept
  {
    return std::pair<uint64_t, uint64_t>(0, std::numeric_limits<uint64_t>::max());
  }

  uint64_t SteamAudioSoundDecoder::getLength() const noexcept
  {
    return 0;
  }

  bool SteamAudioSoundDecoder::seek(uint64_t pos) noexcept
  {
    m_steamaudio->iplFlushDirectSoundEffect(*m_direct_effect);

    return m_decoder->seek(pos);
  }

  ALuint SteamAudioSoundDecoder::read(ALvoid* ptr, ALuint count) noexcept
  {
    alure::Vector<ALubyte> input_data(alure::FramesToBytes(count, m_decoder->getChannelConfig(), m_decoder->getSampleType()));
    auto input_count = m_decoder->read(input_data.data(), count);
    input_data.resize(alure::FramesToBytes(input_count,m_decoder->getChannelConfig(), m_decoder->getSampleType()));

    IPLSource source{ 0.0f, 0.0f, 0.0f };
    auto env = m_mesh_loader->CurrentEnvironment();
    IPLDirectSoundPath sound_path{};
    sound_path = m_steamaudio->iplGetDirectSoundPath(
      *env,
      { 0.0f, 0.0f, 0.0f },
      { 1.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },
      source,
      1.0f,
      10,
      IPLDirectOcclusionMode::IPL_DIRECTOCCLUSION_TRANSMISSIONBYFREQUENCY,
      IPLDirectOcclusionMethod::IPL_DIRECTOCCLUSION_VOLUMETRIC
    );

    IPLDirectSoundEffectOptions opts{};
    opts.applyAirAbsorption = IPL_TRUE;
    opts.applyDistanceAttenuation = IPL_TRUE;
    opts.applyDirectivity = IPL_FALSE;
    opts.directOcclusionMode = IPLDirectOcclusionMode::IPL_DIRECTOCCLUSION_TRANSMISSIONBYFREQUENCY;

    IPLAudioBuffer input{};
    auto input_data_float = alure::ArrayView<ALubyte>(input_data).reinterpret_as<float>();
    input.interleavedBuffer = const_cast<float*>(input_data_float.data());
    input.numSamples = input_count * GetChannelQuantity(m_decoder->getChannelConfig());
    input.format = m_input;

    IPLAudioBuffer output{};
    input.interleavedBuffer = reinterpret_cast<float*>(ptr);
    input.numSamples = count * GetChannelQuantity(OUTPUT_CHANNEL_CONFIG);
    input.format = m_output;

    m_steamaudio->iplApplyDirectSoundEffect(*m_direct_effect, input, sound_path, opts, output);

    return count;
  }


  size_t SteamAudioSoundDecoder::GetChannelQuantity(alure::ChannelConfig channels)
  {
    switch (channels)
    {
    case alure::ChannelConfig::Mono:
      return 1;
    case alure::ChannelConfig::Stereo:
      return 2;
    default:
      throw std::exception("Channel type not implemented.");
    }
  }

  IPLChannelLayout SteamAudioSoundDecoder::GetChannelLayout(alure::ChannelConfig channels)
  {
    switch (channels)
    {
    case alure::ChannelConfig::Mono:
      return IPLChannelLayout::IPL_CHANNELLAYOUT_MONO;
    case alure::ChannelConfig::Stereo:
      return IPLChannelLayout::IPL_CHANNELLAYOUT_STEREO;
    default:
      throw std::exception("Channel type not implemented.");
    }
  }

  IPLChannelLayoutType SteamAudioSoundDecoder::GetChannelLayoutType(alure::ChannelConfig channels)
  {
    switch (channels)
    {
    case alure::ChannelConfig::BFormat2D:
    case alure::ChannelConfig::BFormat3D:
      return IPLChannelLayoutType::IPL_CHANNELLAYOUTTYPE_AMBISONICS;
    default:
      return IPLChannelLayoutType::IPL_CHANNELLAYOUTTYPE_SPEAKERS;
    }
  }
}
