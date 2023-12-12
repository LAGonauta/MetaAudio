#include "Loaders/SoundLoader.hpp"
#include "Voice/VoiceDecoder.hpp"

namespace MetaAudio
{
  SoundLoader::SoundLoader(const std::shared_ptr<AudioCache>& cache)
  {
    m_cache = cache;
    m_decoder = std::make_shared<LocalAudioDecoder>();
  }

  std::optional<alure::String> SoundLoader::S_GetFilePath(const alure::String& sfx_name, bool is_stream)
  {
    alure::String new_name(sfx_name);
    alure::String m_function_name;
    if (is_stream)
    {
      m_function_name = "S_LoadStreamSound";
      new_name.erase(new_name.begin());
    }
    else
    {
      m_function_name = "S_LoadSound";
    }
    bool valid_file = false;

    int char_index = new_name.rfind('.', new_name.length());

    if (char_index != new_name.npos)
    {
      auto context = alure::Context::GetCurrent();
      for (const alure::String& extension : MetaAudio::LocalAudioDecoder::SupportedExtensions)
      {
        new_name.replace(char_index, new_name.npos, extension);
        try
        {
          auto dec = context.createDecoder(new_name);
          valid_file = true;
          break;
        }
        catch (const std::runtime_error& error)
        {
          gEngfuncs.Con_DPrintf("%s: Couldn't load %s. %s.\n", m_function_name.c_str(), new_name.c_str(), error.what());
          valid_file = false;
        }
      }
    }
    else
    {
      gEngfuncs.Con_DPrintf("%s: Couldn't load %s. Invalid file name.\n", m_function_name.c_str(), sfx_name.c_str());
      return std::optional<alure::String>{};
    }

    if (valid_file)
    {
      return new_name;
    }
    else
    {
      gEngfuncs.Con_DPrintf("%s: Couldn't load %s.\n", m_function_name.c_str(), sfx_name.c_str());
      return std::nullopt;
    }
  }

  aud_sfxcache_t* SoundLoader::S_LoadStreamSound(sfx_t* s, aud_channel_t* ch)
  {
    // Some VOX_ that should be stream may not be set as streaming. Fix it here.
    ch->entchannel = CHAN_STREAM;

    std::vector<byte> data;
    aud_sfxcache_t* sc;

    bool ffileopened = false;

    //if (cl.fPrecaching)
    //  return nullptr;

    if (ch == nullptr)
      return nullptr;

    sc = static_cast<aud_sfxcache_t*>(s->cache.data);
    if (sc && sc->decoder)
    {
      ffileopened = true;
    }

    //Alloc cache if we don't have one
    if (sc == nullptr)
    {
      sc = m_cache->Cache_Alloc(&s->cache, s->name);
      if (sc == nullptr)
        return nullptr;
    }

    std::optional<alure::String> file_path;
    if (!ffileopened)
    {
      file_path = S_GetFilePath(s->name, true);
      if (!file_path.has_value())
      {
        return nullptr;
      }
    }

    // For OpenAL
    if (sc->decoder == nullptr && file_path.has_value())
    {
      auto context = alure::Context::GetCurrent();
      try
      {
        sc->decoder = context.createDecoder(file_path.value());
        sc->length = sc->decoder->getLength();
        sc->samplerate = sc->decoder->getFrequency();
        sc->stype = sc->decoder->getSampleType();
        sc->channels = sc->decoder->getChannelConfig();

        auto loop_points = sc->decoder->getLoopPoints();
        sc->looping = sc->decoder->hasLoopPoints();
        sc->loopstart = loop_points.first;
        sc->loopend = loop_points.second;
      }
      catch (const std::exception& error)
      {
        gEngfuncs.Con_DPrintf("S_LoadStreamSound: %s: %s\n", file_path.value(), error.what());
        return nullptr;
      }
    }

    return sc;
  }

  aud_sfxcache_t* SoundLoader::S_LoadSound(sfx_t* s, aud_channel_t* ch)
  {
    try
    {
      aud_sfxcache_t* sc;

      if (s->name[0] == '*')
        return S_LoadStreamSound(s, ch);

      if (s->name[0] == '?')
      {
        try
        {
          auto dec = alure::MakeShared<VoiceDecoder>(s, ch);
          sc = new aud_sfxcache_t();
          sc->channels = dec->getChannelConfig();
          sc->samplerate = dec->getFrequency();
          sc->length = std::numeric_limits<uint64_t>::max();
          sc->looping = false;
          sc->loopstart = 0;
          sc->loopend = std::numeric_limits<uint64_t>::max();
          sc->stype = dec->getSampleType();
          sc->decoder = dec;
          return sc;
        }
        catch (const std::runtime_error& error)
        {
          gEngfuncs.Con_DPrintf("S_LoadSound: Unable to start voice playback. %s.\n", error.what());
          return nullptr;
        }
      }

      sc = static_cast<aud_sfxcache_t*>(s->cache.data);
      if (sc)
        return sc;

      auto context = alure::Context::GetCurrent();
      if (m_decoder != context.getMessageHandler())
      {
        context.setMessageHandler(m_decoder);
      }

      auto file_path = S_GetFilePath(s->name, false);
      if (!file_path)
      {
        return nullptr;
      }

      alure::Buffer al_buffer;
      try
      {
        al_buffer = context.getBuffer(file_path.value());
      }
      catch (const std::exception& error)
      {
        gEngfuncs.Con_DPrintf("S_LoadSound: %s: %s\n", file_path.value().c_str(), error.what());
        sc = nullptr;
        return nullptr;
      }

      sc = m_cache->Cache_Alloc(&s->cache, s->name);
      if (sc == nullptr)
        return nullptr;

      wavinfo_t info{};
      //We can't interfere with Alure, so we need a copy of the data for mouth movement.
      if (!m_decoder->GetWavinfo(info, file_path.value(), sc->data))
        return nullptr;

      sc->buffer = al_buffer;
      sc->length = info.samples; //number of samples ( include channels )
      sc->looping = info.looping;
      sc->loopstart = info.loopstart; //-1 or loop start position
      sc->loopend = info.loopend;
      sc->samplerate = info.samplerate;
      sc->stype = info.stype;
      sc->channels = info.channels;

      // Set loop points if needed
      if (sc->looping)
      {
        try
        {
          auto points = sc->buffer.getLoopPoints();
          if (points.first != sc->loopstart)
          {
            sc->buffer.setLoopPoints(static_cast<ALuint>(sc->loopstart), sc->loopend ? static_cast<ALuint>(sc->loopend) : sc->buffer.getLength());
          }
        }
        catch (const std::exception& error)
        {
          gEngfuncs.Con_DPrintf("Unable to set loop points for sound %s. %s. Will use manual looping.\n", s->name, error.what());
          sc->force_streaming = true;
        }
      }

      return sc;
    }
    catch (const std::exception& e)
    {
      MessageBox(NULL, e.what(), "Error on S_LoadSound", MB_ICONERROR);
      exit(0);
    }
  }
}