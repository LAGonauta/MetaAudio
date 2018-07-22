#include <metahook.h>

#include "FileSystem.h"
#include "util.h"
#include "exportfuncs.h"
#include "snd_local.h"
#include "zone.h"

static auto local_decoder = alure::MakeShared<LocalAudioDecoder>();

aud_sfxcache_t *S_LoadStreamSound(sfx_t *s, aud_channel_t *ch)
{
  // Some VOX_ that should be stream may not be set as streaming. Fix it here.
  ch->entchannel = CHAN_STREAM;

  char namebuffer[256];
  std::vector<byte> data;
  aud_sfxcache_t *sc;
  int loadsize;

  qboolean ffileopened = false;

  //if (cl.fPrecaching)
  //	return nullptr;

  if (ch == nullptr)
    return nullptr;

  FileHandle_t file;
  int size;

  //We have FileHandle in cache so just use it to read but open a new handle
  sc = (aud_sfxcache_t *)Cache_Check(&s->cache);
  if (sc && sc->decoder)
  {
    ffileopened = true;
  }

  //Alloc cache if we don't have one
  if (sc == nullptr)
  {
    sc = (aud_sfxcache_t *)Cache_Alloc(&s->cache, sizeof(aud_sfxcache_t), s->name);
    if (sc == nullptr)
      return nullptr;

    //Clear before use
    memset(sc, 0, sizeof(aud_sfxcache_t));
  }

  if (!ffileopened)
  {
    strcpy(namebuffer, "sound/");
    strncat(namebuffer, &s->name[1], sizeof(namebuffer) - strlen(namebuffer) - 1);
    file = g_pFileSystem->Open(namebuffer, "rb");
  }

  loadsize = 0;

  if (!ffileopened)
  {
    size = g_pFileSystem->Size(file);
    // load little data from file to be sure it exists,
    // and get wav information
    loadsize = min(size, 256);

    data.resize(loadsize + 1);
    data[loadsize] = 0;

    g_pFileSystem->Seek(file, 0, FILESYSTEM_SEEK_HEAD);
    g_pFileSystem->Read(data.data(), loadsize, file);

    // always close file
    g_pFileSystem->Close(file);
    file = nullptr;

    if (data.empty())
    {
      gEngfuncs.Con_DPrintf("S_LoadStreamSound: Couldn't load %s\n", s->name);
      return nullptr;
    }
  }

  char al_file_path[MAX_PATH];
  g_pFileSystem->GetLocalPath(namebuffer, al_file_path, sizeof(al_file_path));

  // For OpenAL
  if (sc->decoder == nullptr)
  {
    if (al_file_path != nullptr && al_file_path[0] != 0)
    {
      auto context = alure::Context::GetCurrent();
      try
      {
        sc->decoder = context.createDecoder(al_file_path);
        sc->length = sc->decoder->getLength();
        sc->samplerate = sc->decoder->getFrequency();

        switch (sc->decoder->getSampleType())
        {
        case alure::SampleType::UInt8:
          sc->width = 1;
          break;
        case alure::SampleType::Int16:
          sc->width = 2;
          break;
        case alure::SampleType::Float32:
          sc->width = 4;
          break;
        }
        sc->channels = sc->decoder->getChannelConfig();

        if (sc->decoder->hasLoopPoints())
        {
          auto loop_points = sc->decoder->getLoopPoints();
          sc->loopstart = loop_points.first;
          sc->loopend = loop_points.second;
        }
        else
        {
          sc->loopstart = -1;
          sc->loopend = INT_MAX;
        }
      }
      catch (const std::exception& error)
      {
        gEngfuncs.Con_DPrintf("S_LoadStreamSound: %s: %s\n", namebuffer, error.what());
        return nullptr;
      }
    }
  }

  return sc;
}

aud_sfxcache_t *S_LoadSound(sfx_t *s, aud_channel_t *ch)
{
  if (!openal_enabled)
  {
    return gAudEngine.S_LoadSound(s, ch);
  }

  char namebuffer[256];
  FileHandle_t hFile;
  aud_sfxcache_t *sc;

  if (s->name[0] == '*')
    return S_LoadStreamSound(s, ch);

  if (s->name[0] == '?')
  {
    try
    {
      alure::SharedPtr<VoiceDecoder> dec = alure::MakeShared<VoiceDecoder>(VoiceDecoder(s, ch));
      sc = new aud_sfxcache_t();
      sc->channels = dec->getChannelConfig();
      sc->samplerate = dec->getFrequency();
      sc->length = INT_MAX;
      sc->loopstart = -1;
      sc->loopend = INT_MAX;
      sc->width = 0;
      sc->decoder = std::static_pointer_cast<alure::Decoder>(dec);
      return sc;
    }
    catch (const std::runtime_error& error)
    {
      gEngfuncs.Con_DPrintf("S_LoadSound: Unable to start voice playback. %s.\n", error.what());
      return nullptr;
    }
  }

  sc = (aud_sfxcache_t *)Cache_Check(&s->cache);
  if (sc)
    return sc;

  auto context = alure::Context::GetCurrent();
  if (local_decoder != context.getMessageHandler())
  {
    context.setMessageHandler(local_decoder);
  }

  // Check if file exists. Order: .wav, .flac, .ogg
  alure::String sfx_name = s->name;
  // Search for "." from right to left
  int char_index = sfx_name.size() - 1;
  while (sfx_name[char_index] != '.')
  {
    char_index--;
    if (char_index < 0)
    {
      break;
    }
  }

  if (char_index > 0)
  {
    for (const alure::String &extension : LocalAudioDecoder::SupportedExtensions)
    {
      sfx_name.replace(char_index, sfx_name.npos, extension);

      strcpy(namebuffer, "sound");
      if (s->name[0] != '/')
        strncat(namebuffer, "/", sizeof(namebuffer) - strlen(namebuffer) - 1);
      strncat(namebuffer, sfx_name.c_str(), sizeof(namebuffer) - strlen(namebuffer) - 1);

      hFile = g_pFileSystem->Open(namebuffer, "rb");

      if (!hFile)
      {
        namebuffer[0] = '\0';
        if (s->name[0] != '/')
          strncat(namebuffer, "/", sizeof(namebuffer) - strlen(namebuffer) - 1);
        strncat(namebuffer, sfx_name.c_str(), sizeof(namebuffer) - 1);
        namebuffer[sizeof(namebuffer) - 1] = 0;

        hFile = g_pFileSystem->Open(namebuffer, "rb");
      }

      if (hFile)
      {
        break;
      }
    }
  }

  if (!hFile)
  {
    gEngfuncs.Con_DPrintf("S_LoadSound: Couldn't load %s.\n", namebuffer);
    return nullptr;
  }
  else
  {
    g_pFileSystem->Close(hFile);
    hFile = nullptr;
  }

  wavinfo_t info;

  // For OpenAL
  char al_file_path[MAX_PATH];
  alure::Buffer al_buffer;
  g_pFileSystem->GetLocalPath(namebuffer, al_file_path, sizeof(al_file_path));
  if (al_file_path != nullptr && al_file_path[0] != 0)
  {
    try
    {
      al_buffer = context.getBuffer(al_file_path);
    }
    catch (const std::exception& error)
    {
      gEngfuncs.Con_DPrintf("S_LoadSound: %s: %s\n", namebuffer, error.what());
      sc = nullptr;
      return nullptr;
    }
  }

  alure::Vector<ALubyte> final_data;
  if (!local_decoder->GetWavinfo(&info, al_file_path, final_data))
    return nullptr;

  sc = (aud_sfxcache_t *)Cache_Alloc(&s->cache, sizeof(aud_sfxcache_t) + final_data.size(), s->name);
  if (sc == nullptr)
    return nullptr;

  memset(sc, 0, sizeof(aud_sfxcache_t));

  sc->buffer = alure::MakeShared<alure::Buffer>(al_buffer);
  sc->length = info.samples; //number of samples ( include channels )
  sc->loopstart = info.loopstart; //-1 or loop start position
  sc->loopend = info.loopend;
  sc->samplerate = info.samplerate;
  sc->width = info.width;
  sc->channels = info.channels;

  //For VOX_ usage. We need a copy of the audio data to not interfere with Alure.
  memcpy(sc->data, final_data.data(), final_data.size());
  final_data.clear();

  // Set loop points if needed
  if (sc->loopstart > 0)
  {
    try
    {
      auto points = sc->buffer->getLoopPoints();
      if (points.first != sc->loopstart)
      {
        sc->buffer->setLoopPoints(sc->loopstart, sc->loopend ? sc->loopend : sc->buffer->getLength());
      }
    }
    catch (const std::exception& error)
    {
      gEngfuncs.Con_DPrintf("Unable to set loop points for sound %s. %s. Will use manual looping.\n", s->name, error.what());
    }
  }

  return sc;
}