#include <metahook.h>
#include <algorithm>
#include <regex>
#include <cctype>

#include "Vox/VoxManager.hpp"
#include "SoundSources/BaseSoundSource.hpp"

namespace MetaAudio
{
  VoxManager::VoxManager(AudioEngine* engine, std::shared_ptr<SoundLoader> loader)
    : m_engine(engine), m_loader(loader)
  {}

  void VoxManager::TrimStartEndTimes(aud_channel_t* ch, aud_sfxcache_t* sc)
  {
    size_t srcsample;
    int skiplen;

    if (ch->words.size() == 0)
    {
      return;
    }

    // only mono support
    if (sc->channels != alure::ChannelConfig::Mono)
    {
      return;
    }

    auto pvoxword = &ch->words.front();
    pvoxword->cbtrim = sc->length;

    auto sstart = static_cast<float>(pvoxword->start);
    auto send = static_cast<float>(pvoxword->end);
    auto length = static_cast<size_t>(sc->length);

    if (ch->entchannel == CHAN_STREAM)
    {
      auto start = length * (sstart / 100);
      if (sstart > 0 && sstart < 100)
      {
        if (start < length)
        {
          ch->start += static_cast<int>(start);
        }
      }
      return;
    }

    if (sstart > send)
      return;

    alure::ArrayView<ALubyte> data_viewer = sc->data;
    if (sstart > 0 && sstart < 100)
    {
      skiplen = static_cast<int>(length * (sstart / 100));
      srcsample = static_cast<size_t>(ch->start);
      ch->start += skiplen;

      if (!sc->data.empty() && ch->start < length)
      {
        switch (sc->stype)
        {
        case alure::SampleType::UInt8:
        {
          auto temp_viewer = data_viewer.reinterpret_as<ALubyte>();
          for (size_t i = 0; i < CVOXZEROSCANMAX; ++i)
          {
            if (srcsample >= sc->length)
              break;

            if (temp_viewer[srcsample] + SCHAR_MIN >= -2 && temp_viewer[srcsample] + SCHAR_MIN <= 2)
            {
              ch->start += 1;
              break;
            }

            srcsample++;
          }
          break;
        }
        case alure::SampleType::Int16:
        {
          auto temp_viewer = data_viewer.reinterpret_as<int16>();
          for (size_t i = 0; i < CVOXZEROSCANMAX; ++i)
          {
            if (srcsample >= sc->length)
              break;

            if (temp_viewer[srcsample] >= -512 && temp_viewer[srcsample] <= 512)
            {
              ch->start += 1;
              break;
            }

            srcsample++;
          }
          break;
        }
        case alure::SampleType::Float32:
        {
          auto temp_viewer = data_viewer.reinterpret_as<float>();
          for (size_t i = 0; i < CVOXZEROSCANMAX; ++i)
          {
            if (srcsample >= sc->length)
              break;

            if (temp_viewer[srcsample] >= -0.016 && temp_viewer[srcsample] <= 0.016)
            {
              ch->start += 1;
              break;
            }

            srcsample++;
          }
          break;
        }
        }
      }

      if (pvoxword->pitch != 100)
        pvoxword->samplefrac += ch->start << 8;
    }

    if (send > 0 && send <= 100)
    {
      skiplen = static_cast<int>(sc->length * ((100 - send) / 100));
      length -= skiplen;
      srcsample = length;
      ch->end -= skiplen;
      pvoxword->cbtrim -= skiplen;

      if (!sc->data.empty() && ch->start < length)
      {
        switch (sc->stype)
        {
        case alure::SampleType::UInt8:
        {
          auto temp_viewer = data_viewer.reinterpret_as<ALubyte>();
          for (size_t i = 0; i < CVOXZEROSCANMAX; ++i)
          {
            if (srcsample <= ch->start)
              break;

            if (temp_viewer[srcsample] + SCHAR_MIN >= -2 && temp_viewer[srcsample] + SCHAR_MIN <= 2)
            {
              ch->end -= 1;
              pvoxword->cbtrim -= 1;
            }
            else
            {
              break;
            }

            srcsample--;
          }
          break;
        }
        case alure::SampleType::Int16:
        {
          auto temp_viewer = data_viewer.reinterpret_as<int16>();
          for (size_t i = 0; i < CVOXZEROSCANMAX; ++i)
          {
            if (srcsample <= ch->start)
              break;

            if (temp_viewer[srcsample] >= -512 && temp_viewer[srcsample] <= 512)
            {
              ch->end -= 1;
              pvoxword->cbtrim -= 1;
            }
            else
            {
              break;
            }

            srcsample--;
          }
          break;
        }
        case alure::SampleType::Float32:
        {
          auto temp_viewer = data_viewer.reinterpret_as<float>();
          for (size_t i = 0; i < CVOXZEROSCANMAX; ++i)
          {
            if (srcsample <= ch->start)
              break;

            if (temp_viewer[srcsample] >= -0.016 && temp_viewer[srcsample] <= 0.016)
            {
              ch->end -= 1;
              pvoxword->cbtrim -= 1;
            }
            else
            {
              break;
            }

            srcsample--;
          }
          break;
        }
        }
      }
    }
  }

  void VoxManager::SetChanVolPitch(aud_channel_t* ch, float* fvol, float* fpitch)
  {
    int vol, pitch;

    if (ch->words.size() == 0)
      return;

    vol = ch->words.front().volume;

    if (vol > 0 && vol != 100)
    {
      (*fvol) *= (vol / 100.0f);
    }

    pitch = ch->words.front().pitch;

    if (pitch > 0 && pitch != 100)
    {
      (*fpitch) *= (pitch / 100.0f);
    }
  }

  std::optional<alure::String> VoxManager::LookupString(const alure::String& pszin, int* psentencenum)
  {
    char* cptr;
    sentenceEntry_s* sentenceEntry;

    if (pszin[0] == '#')
    {
      sentenceEntry = gAudEngine.SequenceGetSentenceByIndex(std::atoi(pszin.c_str() + 1));

      if (sentenceEntry)
        return alure::String(sentenceEntry->data);
    }

    for (size_t i = 0, final = *gAudEngine.cszrawsentences; i < final; ++i)
    {
      if (!_stricmp(pszin.c_str(), (*gAudEngine.rgpszrawsentence)[i]))
      {
        if (psentencenum)
          *psentencenum = i;

        cptr = &(*gAudEngine.rgpszrawsentence)[i][strlen((*gAudEngine.rgpszrawsentence)[i]) + 1];
        while (*cptr == ' ' || *cptr == '\t')
          cptr++;

        return alure::String(cptr);
      }
    }
    return std::nullopt;
  }

  alure::String VoxManager::GetDirectory(alure::String& szpath, alure::String& psz)
  {
    if (psz.length() == 0)
    {
      return alure::String();
    }
    int charscan_index = psz.length() - 1;

    // scan backwards until first '/' or start of string
    while (charscan_index > 0 && psz[charscan_index] != '/')
    {
      --charscan_index;
    }

    if (psz[charscan_index] != '/')
    {
      // didn't find '/', return default directory
      szpath = "vox/";
      return psz;
    }

    szpath.assign(psz, 0, charscan_index + 1);
    return alure::String(psz, charscan_index + 1, psz.length());
  }

  // Regex for matching:
  // 1. Comma and periods
  // 2. Words with parameters and just parameteres. For ex.: (p110 t40) clik!(p120)
  // 3. Full words. For ex.: clik
  static const std::regex regex_match_vox(R"([,.]|[^\s,.]*\(.+?\)|\b[^\s,.]*[^\s.,])");
  alure::Vector<alure::String> VoxManager::ParseString(const alure::String& psz)
  {
    std::vector<alure::String> words;

    if (!psz.length())
      return words;

    auto matches_begin = std::sregex_iterator(psz.cbegin(), psz.cend(), regex_match_vox);
    auto matches_end = std::sregex_iterator();
    while (matches_begin != matches_end) {
      auto& match = *matches_begin;
      const alure::String& sub_match = match[0];
      if (sub_match == ",")
      {
        words.push_back(voxcomma);
      }
      else if (sub_match == ".")
      {
        words.push_back(voxperiod);
      }
      else
      {
        words.push_back(sub_match);
      }
      ++matches_begin;
    }

    return words;
  }

  std::optional<voxword_t> VoxManager::ParseWordParams(alure::String& initial_string, int fFirst)
  {
    size_t charscan_index;
    alure::String psz = initial_string;

    // init to defaults if this is the first word in string.
    if (fFirst)
    {
      voxwordDefault = voxword_t{ 0 };
      voxwordDefault.pitch = -1;
      voxwordDefault.volume = 100;
      voxwordDefault.end = 100;
    }

    auto voxword = voxwordDefault;

    // look at next to last char to see if we have a
    // valid format:
    charscan_index = psz.length() - 1;

    if (psz[charscan_index] != ')')
      return voxword;  // no formatting, return

    // scan forward to first '('
    charscan_index = 0;
    while (charscan_index < psz.length() && !(psz[charscan_index] == '(' || psz[charscan_index] == ')'))
      ++charscan_index;

    if (psz[charscan_index] == ')')
      return std::nullopt;  // bogus formatting

    // remove parameter block from initial string
    initial_string.assign(initial_string, 0, charscan_index);

    alure::String ct; ct = psz[++charscan_index];
    auto ValidCharacter = [](char& character) { return character == 'v' || character == 'p' || character == 's' || character == 'e' || character == 't'; };
    while (1)
    {
      // scan until we hit a character in the commandSet
      while (charscan_index < psz.length() && psz[charscan_index] != ')' &&
             !ValidCharacter(ct[0]))
      {
        ct = psz[++charscan_index];
      }

      if (psz[charscan_index] == ')')
        break;

      ++charscan_index;
      if (!std::isdigit(psz[charscan_index]))
        break;

      // read number
      alure::String sznum = "";
      while (std::isdigit(psz[charscan_index]))
      {
        sznum += psz[charscan_index];
        ++charscan_index;
      }

      switch (ct[0])
      {
      case 'v': voxword.volume = std::stoi(sznum); break;
      case 'p': voxword.pitch = std::stoi(sznum); break;
      case 's': voxword.start = std::stoi(sznum); break;
      case 'e': voxword.end = std::stoi(sznum); break;
      case 't': voxword.timecompress = std::stoi(sznum); break;
      }

      ct = psz[charscan_index];
    }

    // isolated parameter block
    if (psz[0] == '(')
    {
      voxwordDefault = voxword;
      return std::nullopt;
    }
    else
      return voxword;
  }

  aud_sfxcache_t* VoxManager::LoadSound(aud_channel_t* channel, const alure::String& pszin)
  {
    auto check_spaces = [](auto& value) { return std::isspace(value); };

    if (pszin.empty() ||
        std::all_of(pszin.begin(), pszin.end(), check_spaces))
    {
      return nullptr;
    }

    // lookup actual string in (*gAudEngine.rgpszrawsentence),
    // set pointer to string data
    alure::String psz;
    auto data_string = LookupString(pszin, nullptr);
    if (data_string.has_value())
    {
      psz = data_string.value();
    }
    else
    {
      gEngfuncs.Con_DPrintf("VOX_LoadSound: no sentence named %s\n", pszin);
      return nullptr;
    }

    // get directory from string, advance psz
    alure::String szpath;
    psz = GetDirectory(szpath, psz);

    // parse sentence
    auto words = ParseString(psz);

    // for each word in the sentence, construct the filename,
    // lookup the sfx and save each pointer in a temp array
    for (size_t i = 0, final = words.size(); i < final; ++i)
    {
      // Get any pitch, volume, start, end params into voxword
      if (auto voxParameter = ParseWordParams(words[i], i == 0))
      {
        auto& value = voxParameter.value();
        // this is a valid word (as opposed to a parameter block)
        auto pathbuffer = szpath + words[i] + ".wav";

        // find name, if already in cache, mark voxword
        // so we don't discard when word is done playing
        value.sfx = m_engine->S_FindName(const_cast<char*>(pathbuffer.c_str()), &value.fKeepCached);

        channel->words.emplace(value);
      }
    }

    channel->sfx = channel->words.size() > 0 ? channel->words.front().sfx : nullptr;
    channel->vox = this;

    if (!channel->sfx)
    {
      return nullptr;
    }

    auto sc = m_loader->S_LoadSound(channel->sfx, channel);
    if (!sc)
    {
      return nullptr;
    }

    return sc;
  }

  void VoxManager::ForceInitMouth(int entnum)
  {
    cl_entity_t* pEntity = gEngfuncs.GetEntityByIndex(entnum);

    if (pEntity)
    {
      // init mouth movement vars
      pEntity->mouth.mouthopen = 0;
      pEntity->mouth.sndavg = 0;
      pEntity->mouth.sndcount = 0;
    }
  }

  void VoxManager::ForceCloseMouth(int entnum)
  {
    cl_entity_t* pEntity = gEngfuncs.GetEntityByIndex(entnum);

    if (pEntity)
    {
      pEntity->mouth.mouthopen = 0;
    }
  }

  void VoxManager::InitMouth(int entnum, int entchannel)
  {
    if (entchannel == CHAN_STREAM || entchannel == CHAN_VOICE)
      ForceInitMouth(entnum);
  }

  void VoxManager::CloseMouth(aud_channel_t* ch)
  {
    if (ch->entchannel == CHAN_VOICE || ch->entchannel == CHAN_STREAM)
      ForceCloseMouth(ch->entnum);
  }

  void VoxManager::MoveMouth(aud_channel_t* ch, aud_sfxcache_t* sc)
  {
    int data;
    size_t i;
    int savg;
    int scount;
    cl_entity_t* pent;

    pent = gEngfuncs.GetEntityByIndex(ch->entnum);

    if (!pent)
      return;

    i = static_cast<size_t>(ch->sound_source->GetSampleOffset());
    scount = pent->mouth.sndcount;
    savg = 0;

    alure::ArrayView<ALubyte> data_viewer = sc->data;
    switch (sc->stype)
    {
    case alure::SampleType::UInt8:
    {
      auto temp_viewer = data_viewer.reinterpret_as<ALubyte>();
      while (i < sc->length && scount < CAVGSAMPLES)
      {
        data = temp_viewer[i] + SCHAR_MIN;
        savg += abs(data);

        i += 80 + ((byte)data & 0x1F);
        scount++;
      }
      break;
    }
    case alure::SampleType::Int16:
    {
      auto temp_viewer = data_viewer.reinterpret_as<int16>();
      while (i < sc->length && scount < CAVGSAMPLES)
      {
        data = std::clamp(temp_viewer[i] >> 8, SCHAR_MIN, SCHAR_MAX);
        savg += abs(data);

        i += 80 + ((byte)data & 0x1F);
        scount++;
      }
      break;
    }
    case alure::SampleType::Float32:
    {
      auto temp_viewer = data_viewer.reinterpret_as<float>();
      while (i < sc->length && scount < CAVGSAMPLES)
      {
        data = std::clamp(static_cast<int>(temp_viewer[i] * 128), SCHAR_MIN, SCHAR_MAX);
        savg += abs(data);

        i += 80 + ((byte)data & 0x1F);
        scount++;
      }
      break;
    }
    }

    pent->mouth.sndavg += savg;
    pent->mouth.sndcount = (byte)scount;

    if (pent->mouth.sndcount >= CAVGSAMPLES)
    {
      pent->mouth.mouthopen = pent->mouth.sndavg / CAVGSAMPLES;
      pent->mouth.sndavg = 0;
      pent->mouth.sndcount = 0;
    }
  }
}