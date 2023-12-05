#include <metahook.h>

#include "Loaders/LocalAudioDecoder.hpp"

namespace MetaAudio
{
  const alure::Array<alure::String, 4> LocalAudioDecoder::SupportedExtensions = { ".wav", ".flac", ".ogg", ".mp3" };

  bool LocalAudioDecoder::GetWavinfo(wavinfo_t& info, alure::String full_path, alure::Vector<ALubyte>& data_output)
  {
    auto context = alure::Context::GetCurrent();
    alure::SharedPtr<alure::Decoder> dec;
    try
    {
      dec = context.createDecoder(full_path);
    }
    catch (const std::exception& error)
    {
      gEngfuncs.Con_DPrintf("Unable to create decoder for %s. %s.", full_path.c_str(), error.what());
      return false;
    }

    auto& audioData = m_data[full_path];
    info = audioData.info;

    auto loop_points = dec->getLoopPoints();
    info.looping = dec->hasLoopPoints();
    info.loopstart = loop_points.first;
    info.loopend = loop_points.second;

    data_output.swap(audioData.data);
    m_data.erase(full_path);
    return true;
  }

  void LocalAudioDecoder::bufferLoading(alure::StringView name, alure::ChannelConfig channels, alure::SampleType type, ALuint samplerate, alure::ArrayView<ALbyte> data) noexcept
  {
    auto temp_view = data.reinterpret_as<ALubyte>();
    Audio audio{};
    audio.data = alure::Vector<ALubyte>(temp_view.begin(), temp_view.end());
    audio.info.channels = channels;
    audio.info.samplerate = samplerate;
    audio.info.stype = type;
    audio.info.samples = audio.data.size() / alure::FramesToBytes(1, audio.info.channels, audio.info.stype);
    m_data.emplace(alure::String(name.data()), audio);
  }
}