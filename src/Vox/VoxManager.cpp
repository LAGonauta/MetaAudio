#include <metahook.h>
#include <cctype>

#include "Vox/VoxManager.hpp"

namespace MetaAudio
{
  VoxManager::VoxManager() : rgrgvoxword{} {}

  VoxManager::~VoxManager() {}

  void VoxManager::TrimStartEndTimes(aud_channel_t* ch, aud_sfxcache_t* sc)
  {
    float sstart;
    float send;
    size_t length;
    int i;
    size_t srcsample;
    int skiplen;
    voxword_t* pvoxword;

    if (ch->isentence < 0)
      return;

    // only mono support
    if (sc->channels != alure::ChannelConfig::Mono)
      return;

    pvoxword = &rgrgvoxword[ch->isentence][ch->iword];
    pvoxword->cbtrim = sc->length;

    sstart = static_cast<float>(pvoxword->start);
    send = static_cast<float>(pvoxword->end);
    length = static_cast<size_t>(sc->length);

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
          for (i = 0; i < CVOXZEROSCANMAX; i++)
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
          for (i = 0; i < CVOXZEROSCANMAX; i++)
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
          for (i = 0; i < CVOXZEROSCANMAX; i++)
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
          for (i = 0; i < CVOXZEROSCANMAX; i++)
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
          for (i = 0; i < CVOXZEROSCANMAX; i++)
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
          for (i = 0; i < CVOXZEROSCANMAX; i++)
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

    if (ch->isentence < 0)
      return;

    vol = rgrgvoxword[ch->isentence][ch->iword].volume;

    if (vol > 0 && vol != 100)
    {
      (*fvol) *= (vol / 100.0f);
    }

    pitch = rgrgvoxword[ch->isentence][ch->iword].pitch;

    if (pitch > 0 && pitch != 100)
    {
      (*fpitch) *= (pitch / 100.0f);
    }
  }

  std::optional<alure::String> VoxManager::LookupString(const alure::String& pszin, int* psentencenum)
  {
    int i;
    char* cptr;
    sentenceEntry_s* sentenceEntry;

    if (pszin[0] == '#')
    {
      sentenceEntry = gAudEngine.SequenceGetSentenceByIndex(std::atoi(pszin.c_str() + 1));

      if (sentenceEntry)
        return alure::String(sentenceEntry->data);
    }

    for (i = 0; i < *gAudEngine.cszrawsentences; i++)
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

  void VoxManager::ParseString(const alure::String& psz)
  {
    rgpparseword.fill(alure::String());

    if (!psz.length())
      return;

    // Split over space, comma, and periods.
    // If comma or periods, add them to own slot.
    int psz_size = psz.length();
    int charscan_initial_index = 0;
    int charscan_last_index = 0;
    size_t word_index = 0;
    while (word_index < CVOXWORDMAX && charscan_last_index < psz_size)
    {
      if (psz[charscan_last_index] == ' ')
      {
        if ((charscan_last_index - 1 >= 0) && psz[charscan_last_index - 1] != ',' && psz[charscan_last_index - 1] != '.')
        {
          rgpparseword[word_index].assign(psz, charscan_initial_index, charscan_last_index - charscan_initial_index);
          ++word_index;
        }
        charscan_initial_index = charscan_last_index + 1;
      }
      else if (psz[charscan_last_index] == '(')
      {
        // search for matching ')'
        while (charscan_last_index < psz.length() && psz[charscan_last_index] != ')')
        {
          ++charscan_last_index;
        }
      }
      else if (psz[charscan_last_index] == '.' || psz[charscan_last_index] == ',')
      {
        if ((charscan_last_index - 1 >= 0) && psz[charscan_last_index - 1] != ' ')
        {
          rgpparseword[word_index].assign(psz, charscan_initial_index, charscan_last_index - charscan_initial_index);
          ++word_index;
        }

        switch (psz[charscan_last_index])
        {
        case ',':
          rgpparseword[word_index].assign(voxcomma);
          ++word_index;
          break;
        case '.':
          rgpparseword[word_index].assign(voxperiod);
          ++word_index;
          break;
        }
        charscan_initial_index = charscan_last_index + 1;
      }
      ++charscan_last_index;

      // Finished parsing, add last word
      if (charscan_last_index == psz_size)
      {
        rgpparseword[word_index].assign(psz, charscan_initial_index, charscan_last_index - charscan_initial_index);
        ++word_index;
      }
    }
  }

  bool VoxManager::ParseWordParams(alure::String& initial_string, voxword_t* pvoxword, int fFirst)
  {
    size_t charscan_index;
    alure::String psz = initial_string;
    alure::String ct;
    alure::String sznum;

    // init to defaults if this is the first word in string.
    if (fFirst)
    {
      voxwordDefault = voxword_t{ 0 };
      voxwordDefault.pitch = -1;
      voxwordDefault.volume = 100;
      voxwordDefault.end = 100;
    }

    *pvoxword = voxwordDefault;

    // look at next to last char to see if we have a
    // valid format:
    charscan_index = psz.length() - 1;

    if (psz[charscan_index] != ')')
      return true;  // no formatting, return

    // scan forward to first '('
    charscan_index = 0;
    while (charscan_index < psz.length() && !(psz[charscan_index] == '(' || psz[charscan_index] == ')'))
      ++charscan_index;

    if (psz[charscan_index] == ')')
      return false;  // bogus formatting

    // remove parameter block from initial string
    initial_string.assign(initial_string, 0, charscan_index);

    ct = psz[++charscan_index];

    while (1)
    {
      // scan until we hit a character in the commandSet
      while (charscan_index < psz.length() && psz[charscan_index] != ')' &&
        !(ct[0] == 'v' || ct[0] == 'p' ||
          ct[0] == 's' || ct[0] == 'e' ||
          ct[0] == 't'))
      {
        ct = psz[++charscan_index];
      }

      if (psz[charscan_index] == ')')
        break;

      ++charscan_index;
      if (!isdigit(psz[charscan_index]))
        break;

      // read number
      sznum = "";
      while (isdigit(psz[charscan_index]))
      {
        sznum += psz[charscan_index];
        ++charscan_index;
      }

      switch (ct[0])
      {
      case 'v': pvoxword->volume = stoi(sznum); break;
      case 'p': pvoxword->pitch = stoi(sznum); break;
      case 's': pvoxword->start = stoi(sznum); break;
      case 'e': pvoxword->end = stoi(sznum); break;
      case 't': pvoxword->timecompress = stoi(sznum); break;
      }

      ct = psz[charscan_index];
    }

    // isolated parameter block
    if (psz[0] == '(')
    {
      voxwordDefault = *pvoxword;
      return false;
    }
    else
      return true;
  }

  int VoxManager::IFindEmptySentence(void)
  {
    int k;

    for (k = 0; k < CVOXSENTENCEMAX; k++)
    {
      if (!rgrgvoxword[k][0].sfx)
        return k;
    }

    gEngfuncs.Con_DPrintf("Sentence or Pitch shift ignored. > 16 playing!\n");
    return -1;
  }

  aud_sfxcache_t* VoxManager::LoadSound(aud_channel_t* pchan, const alure::String& pszin)
  {
    if (pszin.empty() || std::all_of(pszin.begin(), pszin.end(), std::isspace))
    {
      return nullptr;
    }

    int i, j, k, cword;
    alure::String pathbuffer;
    alure::String szpath;
    aud_sfxcache_t* sc;
    alure::Array<voxword_t, CVOXWORDMAX> rgvoxword{};
    alure::String psz;

    // lookup actual string in (*gAudEngine.rgpszrawsentence),
    // set pointer to string data
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
    psz = GetDirectory(szpath, psz);

    // parse sentence
    ParseString(psz);

    // for each word in the sentence, construct the filename,
    // lookup the sfx and save each pointer in a temp array
    i = 0;
    cword = 0;
    while (rgpparseword[i].length())
    {
      // Get any pitch, volume, start, end params into voxword
      if (ParseWordParams(rgpparseword[i], &rgvoxword[cword], i == 0))
      {
        // this is a valid word (as opposed to a parameter block)
        pathbuffer = szpath + rgpparseword[i] + ".wav";

        // find name, if already in cache, mark voxword
        // so we don't discard when word is done playing
        rgvoxword[cword].sfx = S_FindName(const_cast<char*>(pathbuffer.c_str()), &(rgvoxword[cword].fKeepCached));
        cword++;
      }
      i++;
    }

    k = IFindEmptySentence();
    if (k < 0)
      return nullptr;

    j = 0;
    while (rgvoxword[j].sfx != nullptr)
    {
      rgrgvoxword[k][j] = rgvoxword[j];
      ++j;
    }

    pchan->isentence = k;
    pchan->iword = 0;
    pchan->sfx = rgvoxword[0].sfx;

    if (!pchan->sfx)
    {
      S_FreeChannel(pchan);
      return nullptr;
    }

    sc = S_LoadSound(pchan->sfx, pchan);
    if (!sc)
    {
      S_FreeChannel(pchan);
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

    i = static_cast<size_t>(ch->source.getSampleOffset());
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