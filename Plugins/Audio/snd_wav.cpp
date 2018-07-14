#include <metahook.h>
#include <cvardef.h>
#include "plugins.h"
#include "exportfuncs.h"
#include "enginedef.h"
#include "plat.h"
#include "snd_local.h"
#include "zone.h"

short LocalAudioDecoder::GetLittleShort(void)
{
  short val = 0;
  val = *data_p;
  val = val + (*(data_p + 1) << 8);
  data_p += 2;
  return val;
}

int LocalAudioDecoder::GetLittleLong(void)
{
  int val = 0;
  val = *data_p;
  val = val + (*(data_p + 1) << 8);
  val = val + (*(data_p + 2) << 16);
  val = val + (*(data_p + 3) << 24);
  data_p += 4;
  return val;
}

void LocalAudioDecoder::FindNextChunk(char *name)
{
  while (1)
  {
    data_p = last_chunk;

    if (data_p >= iff_end)
    {	// didn't find the chunk
      data_p = NULL;
      return;
    }

    data_p += 4;
    iff_chunk_len = GetLittleLong();
    if (iff_chunk_len < 0)
    {
      data_p = NULL;
      return;
    }
    //		if (iff_chunk_len > 1024*1024)
    //			Sys_Error ("FindNextChunk: %i length is past the 1 meg sanity limit", iff_chunk_len);
    data_p -= 8;
    last_chunk = data_p + 8 + ((iff_chunk_len + 1) & ~1);
    if (!strncmp((const char *)data_p, name, 4))
      return;
  }
}

void LocalAudioDecoder::FindChunk(char *name)
{
  last_chunk = iff_data;
  FindNextChunk(name);
}

bool LocalAudioDecoder::GetWavinfo(wavinfo_t *info, char *path, byte *wav, int wavlength, alure::ArrayView<ALbyte>& data_output)
{
  int     i;

  memset(info, 0, sizeof(*info));

  if (!wav)
    return false;

  iff_data = wav;
  iff_end = wav + wavlength;

  // If wav files try to find looppoints
  // Else believe in Alure
  bool wav_file = true;

  // find "RIFF" chunk
  FindChunk("RIFF");
  if (!(data_p && !strncmp((const char *)(data_p + 8), "WAVE", 4)))
  {
    wav_file =  false;
  }

  if (wav_file)
  {
    // get "fmt " chunk
    iff_data = data_p + 12;

    FindChunk("fmt ");
    if (!data_p)
    {
      gEngfuncs.Con_Printf("Missing fmt chunk\n");
      return false;
    }

    // skips format, channels, samplerate, bps, aling, and width chunks
    data_p += 22;

    // get cue chunk
    FindChunk("cue ");
    if (data_p)
    {
      data_p += 8; // skip id and data size
      int num_cue_pts = GetLittleLong();
      std::vector<int> loop_pts;
      for (int i = 0; i < num_cue_pts; ++i)
      {
        data_p += 20; // useless data for us
        loop_pts.push_back(GetLittleLong());
      }

      // We only support the first two looping points
      if (num_cue_pts > 0)
      {
        info->loopstart = loop_pts[0];
        if (num_cue_pts > 1)
        {
          info->loopend = loop_pts[1];
        }
        else
        {
          info->loopend = INT_MAX;
        }
      }

      // if the next chunk is a LIST chunk, look for a cue length marker
      FindNextChunk("LIST");
      if (data_p)
      {
        if (!strncmp((const char *)(data_p + 28), "mark", 4))
        {	// this is not a proper parse, but it works with cooledit...
          data_p += 24;
          i = GetLittleLong();	// samples in loop
          info->samples = info->loopstart + i;
        }
      }
    }
    else
    {
      info->loopstart = -1;
      info->loopend = INT_MAX;
    }
  }
  else
  {
    auto context = alure::Context::GetCurrent();
    alure::SharedPtr<alure::Decoder> dec = context.createDecoder(path);

    auto loop_points = dec->getLoopPoints();

    // How to know it has no cue points? Every sound will loop with this setting here.
    // Alure2 API suggestion: decoder.hasLoopPoints();
    info->loopstart = loop_points.first;
    info->loopend = loop_points.second;
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