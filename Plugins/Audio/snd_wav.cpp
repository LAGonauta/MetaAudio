#include <metahook.h>
#include <cvardef.h>
#include "plugins.h"
#include "exportfuncs.h"
#include "enginedef.h"
#include "plat.h"
#include "snd_local.h"
#include "zone.h"

bool LocalAudioDecoder::GetWavinfo(wavinfo_t *info, char *full_path, alure::ArrayView<ALbyte>& data_output)
{
  memset(info, 0, sizeof(*info));

  auto context = alure::Context::GetCurrent();
  alure::SharedPtr<alure::Decoder> dec;
  try
  {
    dec = context.createDecoder(full_path);
  }
  catch (const std::exception& error)
  {
    return false;
  }
  
  if (dec->hasLoopPoints())
  {
    auto loop_points = dec->getLoopPoints();
    info->loopstart = loop_points.first;
    info->loopend = loop_points.second;
  }
  else
  {
    info->loopstart = -1;
    info->loopend = INT_MAX;
  }

  info->channels = _channels;
  info->samplerate = _samplerate;

  switch (_type)
  {
  case alure::SampleType::UInt8:
    info->width = 1;
    break;
  case alure::SampleType::Int16:
    info->width = 2;
    break;
  case alure::SampleType::Float32:
    info->width = 4;
    break;
  }

  info->samples = _data.size() / alure::FramesToBytes(1, _channels, _type);
  data_output = _data;

  return true;
}

void LocalAudioDecoder::bufferLoading(alure::StringView name, alure::ChannelConfig channels, alure::SampleType type, ALuint samplerate, alure::ArrayView<ALbyte> data) noexcept {
  _name = name;
  _type = type;
  _samplerate = samplerate;
  _data = data;
}