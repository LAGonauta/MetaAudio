#include <metahook.h>

#include "exportfuncs.h"
#include "FileSystem.h"
#include "snd_local.h"
#include "util.h"
#include "zone.h"

voxword_t rgrgvoxword[CVOXSENTENCEMAX][CVOXWORDMAX];

static char		*rgpparseword[CVOXWORDMAX];	// array of pointers to parsed words
static char		voxperiod[] = "_period";				// vocal pause
static char		voxcomma[] = "_comma";				// vocal pause
static voxword_t voxwordDefault;

void VOX_Init(void)
{
  memset(rgrgvoxword, 0, sizeof(voxword_t) * CVOXSENTENCEMAX * CVOXWORDMAX);
}

void VOX_TrimStartEndTimes(aud_channel_t *ch, aud_sfxcache_t *sc)
{
  float sstart;
  float send;
  int length;
  int i;
  int srcsample;
  int skiplen;
  voxword_t *pvoxword;
  byte *pdata;

  if (ch->isentence < 0)
    return;

  // only mono support
  if (sc->channels != alure::ChannelConfig::Mono)
    return;

  pvoxword = &rgrgvoxword[ch->isentence][ch->iword];
  pvoxword->cbtrim = sc->length;

  sstart = pvoxword->start;
  send = pvoxword->end;
  length = sc->length;

  if (ch->entchannel == CHAN_STREAM)
  {
    auto start = length * (sstart / 100);
    if (sstart > 0 && sstart < 100)
    {
      if (start < length)
      {
        ch->start += start;
      }
    }
    return;
  }

  pdata = (byte *)sc->data.data();

  if (sstart > send)
    return;

  if (sstart > 0 && sstart < 100)
  {
    skiplen = length * (sstart / 100);
    srcsample = ch->start;
    ch->start += skiplen;

    if (pdata != nullptr && ch->start < length)
    {
      switch (sc->width)
      {
      case 1:
      {
        alure::ArrayView<byte> temp_data = sc->data.reinterpret_as<byte>();
        for (i = 0; i < CVOXZEROSCANMAX; i++)
        {
          if (srcsample >= temp_data.size())
            break;

          if (temp_data[srcsample] + SCHAR_MIN >= -2 && temp_data[srcsample] + SCHAR_MIN <= 2)
          {
            ch->start += i;
            break;
          }

          srcsample++;
        }
        break;
      }
      case 2:
      {
        alure::ArrayView<int16> temp_data = sc->data.reinterpret_as<int16>();
        for (i = 0; i < CVOXZEROSCANMAX; i++)
        {
          if (srcsample >= temp_data.size())
            break;

          if (temp_data[srcsample] >= -512 && temp_data[srcsample] <= 512)
          {
            ch->start += i;
            break;
          }

          srcsample++;
        }
        break;
      }
      case 4:
      {
        alure::ArrayView<float> temp_data = sc->data.reinterpret_as<float>();
        for (i = 0; i < CVOXZEROSCANMAX; i++)
        {
          if (srcsample >= temp_data.size())
            break;

          if (temp_data[srcsample] >= -0.016 && temp_data[srcsample] <= 0.016)
          {
            ch->start += i;
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
    skiplen = sc->length * ((100 - send) / 100);
    length -= skiplen;
    srcsample = length;
    ch->end -= skiplen;
    pvoxword->cbtrim -= skiplen;

    if (pdata != nullptr && ch->start < length)
    {
      switch (sc->width)
      {
      case 1:
      {
        alure::ArrayView<byte> temp_data = sc->data.reinterpret_as<byte>();
        for (i = 0; i < CVOXZEROSCANMAX; i++)
        {
          if (srcsample <= ch->start)
            break;

          if (pdata[srcsample] + SCHAR_MIN >= -2 && pdata[srcsample] + SCHAR_MIN <= 2)
          {
            ch->end -= i;
            pvoxword->cbtrim -= i;
          }
          else
          {
            break;
          }

          srcsample--;
        }
        break;
      }
      case 2:
      {
        alure::ArrayView<int16> temp_data = sc->data.reinterpret_as<int16>();
        for (i = 0; i < CVOXZEROSCANMAX; i++)
        {
          if (srcsample >= temp_data.size())
            break;

          if (temp_data[srcsample] >= -512 && temp_data[srcsample] <= 512)
          {
            ch->end -= i;
            pvoxword->cbtrim -= i;
          }
          else
          {
            break;
          }

          srcsample--;
        }
        break;
      }
      case 4:
      {
        alure::ArrayView<float> temp_data = sc->data.reinterpret_as<float>();
        for (i = 0; i < CVOXZEROSCANMAX; i++)
        {
          if (srcsample >= temp_data.size())
            break;

          if (temp_data[srcsample] >= -512 && temp_data[srcsample] <= 512)
          {
            ch->end -= i;
            pvoxword->cbtrim -= i;
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

void VOX_SetChanVolPitch(aud_channel_t *ch, float *fvol, float *fpitch)
{
  int vol, pitch;

  if (ch->isentence < 0)
    return;

  vol = rgrgvoxword[ch->isentence][ch->iword].volume;

  if (vol > 0 && vol != 100)
  {
    (*fvol) *= (vol / 100.0);
  }

  pitch = rgrgvoxword[ch->isentence][ch->iword].pitch;

  if (pitch > 0 && pitch != 100)
  {
    (*fpitch) *= (pitch / 100.0);
  }
}

char *VOX_LookupString(char *pszin, int *psentencenum)
{
  int i;
  char *cptr;
  sentenceEntry_s *sentenceEntry;

  if (pszin[0] == '#')
  {
    const char *indexAsString;

    indexAsString = pszin + 1;
    sentenceEntry = gAudEngine.SequenceGetSentenceByIndex(atoi(indexAsString));

    if (sentenceEntry)
      return sentenceEntry->data;
  }

  for (i = 0; i < *gAudEngine.cszrawsentences; i++)
  {
    if (!stricmp(pszin, (*gAudEngine.rgpszrawsentence)[i]))
    {
      if (psentencenum)
        *psentencenum = i;

      cptr = &(*gAudEngine.rgpszrawsentence)[i][strlen((*gAudEngine.rgpszrawsentence)[i]) + 1];
      while (*cptr == ' ' || *cptr == '\t')
        cptr++;

      return cptr;
    }
  }
  return NULL;
}

char *VOX_GetDirectory(char *szpath, char *psz)
{
  char c;
  int cb = 0;
  char *pszscan = psz + strlen(psz) - 1;

  // scan backwards until first '/' or start of string
  c = *pszscan;
  while (pszscan > psz && c != '/')
  {
    c = *(--pszscan);
    cb++;
  }

  if (c != '/')
  {
    // didn't find '/', return default directory
    strcpy(szpath, "vox/");
    return psz;
  }

  cb = strlen(psz) - cb;
  memcpy(szpath, psz, cb);
  szpath[cb] = 0;
  return pszscan + 1;
}

void VOX_ParseString(char *psz)
{
  int i;
  int fdone = 0;
  char *pszscan = psz;
  char c;

  memset(rgpparseword, 0, sizeof(char *) * CVOXWORDMAX);

  if (!psz)
    return;

  i = 0;
  rgpparseword[i++] = psz;

  while (!fdone && i < CVOXWORDMAX)
  {
    // scan up to next word
    c = *pszscan;
    while (c && !(c == '.' || c == ' ' || c == ',' || c == '('))
      c = *(++pszscan);

    // if '(' then scan for matching ')'
    if (c == '(')
    {
      while (*pszscan != ')')
        pszscan++;

      c = *(++pszscan);
      if (!c)
        fdone = 1;
    }

    if (fdone || !c)
      fdone = 1;
    else
    {
      // if . or , insert pause into rgpparseword,
      // unless this is the last character
      if ((c == '.' || c == ',') && *(pszscan + 1) != '\n' && *(pszscan + 1) != '\r'
        && *(pszscan + 1) != 0)
      {
        if (c == '.')
          rgpparseword[i++] = voxperiod;
        else
          rgpparseword[i++] = voxcomma;

        if (i >= CVOXWORDMAX)
          break;
      }

      // null terminate substring
      *pszscan++ = 0;

      // skip whitespace
      c = *pszscan;
      while (c && (c == '.' || c == ' ' || c == ','))
        c = *(++pszscan);

      if (!c)
        fdone = 1;
      else
        rgpparseword[i++] = pszscan;
    }
  }
}

int VOX_ParseWordParams(char *psz, voxword_t *pvoxword, int fFirst)
{
  char *pszsave = psz;
  char c;
  char ct;
  char sznum[8];
  int i;

  // init to defaults if this is the first word in string.
  if (fFirst)
  {
    voxwordDefault.pitch = -1;
    voxwordDefault.volume = 100;
    voxwordDefault.start = 0;
    voxwordDefault.end = 100;
    voxwordDefault.fKeepCached = 0;
    voxwordDefault.timecompress = 0;
  }

  *pvoxword = voxwordDefault;

  // look at next to last char to see if we have a 
  // valid format:

  c = *(psz + strlen(psz) - 1);

  if (c != ')')
    return 1;		// no formatting, return

                    // scan forward to first '('
  c = *psz;
  while (!(c == '(' || c == ')'))
    c = *(++psz);

  if (c == ')')
    return 0;		// bogus formatting

                    // null terminate

  *psz = 0;
  ct = *(++psz);

  while (1)
  {
    // scan until we hit a character in the commandSet

    while (ct && !(ct == 'v' || ct == 'p' || ct == 's' || ct == 'e' || ct == 't'))
      ct = *(++psz);

    if (ct == ')')
      break;

    memset(sznum, 0, sizeof(sznum));
    i = 0;

    c = *(++psz);

    if (!isdigit(c))
      break;

    // read number
    while (isdigit(c) && i < sizeof(sznum) - 1)
    {
      sznum[i++] = c;
      c = *(++psz);
    }

    // get value of number
    i = atoi(sznum);

    switch (ct)
    {
    case 'v': pvoxword->volume = i; break;
    case 'p': pvoxword->pitch = i; break;
    case 's': pvoxword->start = i; break;
    case 'e': pvoxword->end = i; break;
    case 't': pvoxword->timecompress = i; break;
    }

    ct = c;
  }

  // if the string has zero length, this was an isolated
  // parameter block.  Set default voxword to these
  // values

  if (strlen(pszsave) == 0)
  {
    voxwordDefault = *pvoxword;
    return 0;
  }
  else
    return 1;
}

int VOX_IFindEmptySentence(void)
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

aud_sfxcache_t *VOX_LoadSound(aud_channel_t *pchan, char *pszin)
{
  char buffer[512];
  int i, j, k, cword;
  char	pathbuffer[64];
  char	szpath[32];
  aud_sfxcache_t *sc;
  voxword_t rgvoxword[CVOXWORDMAX];
  char *psz;

  if (!pszin)
    return NULL;

  memset(rgvoxword, 0, sizeof(voxword_t) * CVOXWORDMAX);
  memset(buffer, 0, sizeof(buffer));

  // lookup actual string in (*gAudEngine.rgpszrawsentence), 
  // set pointer to string data

  psz = VOX_LookupString(pszin, NULL);

  if (!psz)
  {
    gEngfuncs.Con_DPrintf("VOX_LoadSound: no sentence named %s\n", pszin);
    return NULL;
  }

  // get directory from string, advance psz
  psz = VOX_GetDirectory(szpath, psz);

  if (strlen(psz) > sizeof(buffer) - 1)
  {
    gEngfuncs.Con_DPrintf("VOX_LoadSound: sentence is too long %s\n", psz);
    return NULL;
  }

  // copy into buffer
  strncpy(buffer, psz, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = 0;
  psz = buffer;

  // parse sentence (also inserts null terminators between words)

  VOX_ParseString(psz);

  // for each word in the sentence, construct the filename,
  // lookup the sfx and save each pointer in a temp array	

  i = 0;
  cword = 0;
  while (rgpparseword[i])
  {
    // Get any pitch, volume, start, end params into voxword

    if (VOX_ParseWordParams(rgpparseword[i], &rgvoxword[cword], i == 0))
    {
      // this is a valid word (as opposed to a parameter block)
      _snprintf(pathbuffer, sizeof(pathbuffer), "%s%s.wav", szpath, rgpparseword[i]);
      pathbuffer[sizeof(pathbuffer) - 1] = 0;

      if (strlen(pathbuffer) >= sizeof(pathbuffer))
        continue;

      // find name, if already in cache, mark voxword
      // so we don't discard when word is done playing
      rgvoxword[cword].sfx = S_FindName(pathbuffer, &(rgvoxword[cword].fKeepCached));
      cword++;
    }
    i++;
  }

  k = VOX_IFindEmptySentence();
  if (k < 0)
    return NULL;

  j = 0;
  while (rgvoxword[j].sfx != NULL)
    rgrgvoxword[k][j] = rgvoxword[j++];

  pchan->isentence = k;
  pchan->iword = 0;
  pchan->sfx = rgvoxword[0].sfx;

  sc = S_LoadSound(pchan->sfx, pchan);
  if (!sc)
  {
    S_FreeChannel(pchan);
    return NULL;
  }

  return sc;
}

//Mouth

void SND_ForceInitMouth(int entnum)
{
  cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(entnum);

  if (pEntity)
  {
    // init mouth movement vars
    pEntity->mouth.mouthopen = 0;
    pEntity->mouth.sndavg = 0;
    pEntity->mouth.sndcount = 0;
  }
}

void SND_ForceCloseMouth(int entnum)
{
  cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(entnum);

  if (pEntity)
  {
    pEntity->mouth.mouthopen = 0;
  }
}

void SND_InitMouth(int entnum, int entchannel)
{
  if (entchannel == CHAN_STREAM || entchannel == CHAN_VOICE)
    SND_ForceInitMouth(entnum);
}

void SND_CloseMouth(aud_channel_t *ch)
{
  if (ch->entchannel == CHAN_VOICE || ch->entchannel == CHAN_STREAM)
    SND_ForceCloseMouth(ch->entnum);
}

void SND_MoveMouth(aud_channel_t *ch, aud_sfxcache_t *sc)
{
  int 	data;
  int		i;
  int		savg;
  int		availableSamples, scount;
  int		iSamplesPlayed;
  cl_entity_t *pent;

  pent = gEngfuncs.GetEntityByIndex(ch->entnum);

  if (!pent)
    return;

  iSamplesPlayed = ch->source.getSampleOffset();
  availableSamples = ch->buffer->getLength() - iSamplesPlayed;

  byte	*pdata = (byte *)sc->data.data() + iSamplesPlayed * sc->width;
  if (pdata == nullptr)
  {
    return;
  }

  i = 0;
  scount = pent->mouth.sndcount;
  savg = 0;

  switch (sc->width)
  {
  case 1:
  {
    alure::ArrayView<byte> temp_array = sc->data.reinterpret_as<byte>();
    while (i < availableSamples && scount < CAVGSAMPLES)
    {
      data = temp_array[i + iSamplesPlayed] + SCHAR_MIN;
      savg += abs(data);

      i += 80 + ((byte)data & 0x1F);
      scount++;
    }
    break;
  }
  case 2:
  {
    alure::ArrayView<int16> temp_array = sc->data.reinterpret_as<int16>();
    while (i < availableSamples && scount < CAVGSAMPLES)
    {
      data = min(max(temp_array[i + iSamplesPlayed] >> 8, SCHAR_MIN + 1), SCHAR_MAX - 1);
      savg += abs(data);
      i += 80 + ((byte)data & 0x1F);
      scount++;
    }
    break;
  }
  case 4:
  {
    alure::ArrayView<float> temp_array = sc->data.reinterpret_as<float>();
    while (i < availableSamples && scount < CAVGSAMPLES)
    {
      data = min(max(temp_array[i + iSamplesPlayed] * 128, SCHAR_MIN + 1), SCHAR_MAX - 1);
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