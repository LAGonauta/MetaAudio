#pragma once

#include "alure2.h"
#include "phonon.h"

#include "DynamicSteamAudio.hpp"
#include "Loaders/SteamAudioMapMeshLoader.hpp"

namespace MetaAudio
{
  class SteamAudioSoundDecoder : public alure::Decoder
  {
  private:
    const size_t FREQUENCY = 48000;
    const alure::ChannelConfig OUTPUT_CHANNEL_CONFIG = alure::ChannelConfig::Stereo; // TODO: add support for optional BFormat3D when context supports it

    size_t m_frame_size = 0;
    alure::SharedPtr<SteamAudio> m_steamaudio = nullptr;
    alure::SharedPtr<SteamAudioMapMeshLoader> m_mesh_loader = nullptr;

    alure::SharedPtr<alure::Decoder> m_decoder = nullptr;
    alure::SharedPtr<IPLhandle> m_binaural_renderer = nullptr;
    alure::SharedPtr<IPLhandle> m_env_renderer = nullptr;

    alure::SharedPtr<IPLhandle> m_binaural_effect;
    alure::SharedPtr<IPLhandle> m_amb_binaural_effect;
    alure::SharedPtr<IPLhandle> m_direct_effect = nullptr;
    alure::SharedPtr<IPLhandle> m_conv_effect;

    size_t GetChannelQuantity(alure::ChannelConfig channels);

    IPLChannelLayout GetChannelLayout(alure::ChannelConfig channels);

    IPLChannelLayoutType GetChannelLayoutType(alure::ChannelConfig channels);

    IPLAudioFormat m_input{};
    IPLAudioFormat m_output{};
    IPLAudioFormat m_intermediate_output{};


  public:
    SteamAudioSoundDecoder(
      alure::SharedPtr<SteamAudio> steamaudio,
      alure::SharedPtr<SteamAudioMapMeshLoader> mesh_loader,
      alure::SharedPtr<alure::Decoder> decoder,
      alure::SharedPtr<IPLhandle> binaural_renderer,
      alure::SharedPtr<IPLhandle> env_renderer,
      size_t frame_size);

    ALuint getFrequency() const noexcept override;
    alure::ChannelConfig getChannelConfig() const noexcept override;
    alure::SampleType getSampleType() const noexcept override;

    bool hasLoopPoints() const noexcept override;
    std::pair<uint64_t, uint64_t> getLoopPoints() const noexcept override;

    uint64_t getLength() const noexcept override;
    bool seek(uint64_t pos) noexcept override;
    ALuint read(ALvoid* ptr, ALuint count) noexcept override;
  };
}