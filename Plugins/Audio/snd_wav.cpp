#include <metahook.h>
#include <cvardef.h>
#include "plugins.h"
#include "exportfuncs.h"
#include "enginedef.h"
#include "plat.h"
#include "snd_local.h"
#include "zone.h"

const alure::Array<alure::String, 3> LocalAudioDecoder::SupportedExtensions = { ".wav", ".flac", ".ogg" };

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
  
  auto loop_points = dec->getLoopPoints();
  info.looping = dec->hasLoopPoints();
  info.loopstart = loop_points.first;
  info.loopend = loop_points.second;

  info.channels = m_channels;
  info.samplerate = m_samplerate;
  info.stype = m_type;
  info.samples = m_data.size() / alure::FramesToBytes(1, m_channels, m_type);

  data_output.swap(m_data);
  return true;
}

void LocalAudioDecoder::bufferLoading(alure::StringView name, alure::ChannelConfig channels, alure::SampleType type, ALuint samplerate, alure::ArrayView<ALbyte> data) noexcept {
  m_name = name;
  m_type = type;
  m_samplerate = samplerate;

  auto temp_view = data.reinterpret_as<ALubyte>();
  m_data = alure::Vector<ALubyte>(temp_view.begin(), temp_view.end());
}