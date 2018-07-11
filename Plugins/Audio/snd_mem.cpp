#include <metahook.h>

#include "alure/AL/alure2.h"
#include "FileSystem.h"
#include "util.h"
#include "exportfuncs.h"
#include "snd_local.h"
#include "zone.h"

aud_sfxcache_t *S_LoadStreamSound(sfx_t *s, aud_channel_t *ch)
{
  // Some VOX_ that should be stream may not be set as streaming. Fix it here.
  ch->entchannel = CHAN_STREAM;

  char			namebuffer[256];
  byte			*data;
  aud_sfxcache_t	*sc;
  int				loadsize;
  wavinfo_t		info;

  qboolean		ffileopened = false;

  //if (cl.fPrecaching)
  //	return nullptr;

  if (ch == nullptr)
    return nullptr;

  //We have FileHandle in cache so just use it to read but open a new handle
  sc = (aud_sfxcache_t *)Cache_Check(&s->cache);
  if (sc && sc->file)
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
    sc->file = g_pFileSystem->Open(namebuffer, "rb");
  }

  data = nullptr;
  loadsize = 0;

  if (sc->file)
  {
    sc->filesize = g_pFileSystem->Size(sc->file);
    // load little data from file to be sure it exists,
    // and get wav information
    loadsize = min(sc->filesize, 256);

    data = (byte *)Hunk_TempAlloc(loadsize + 1);
    data[loadsize] = 0;

    g_pFileSystem->Seek(sc->file, 0, FILESYSTEM_SEEK_HEAD);
    g_pFileSystem->Read(data, loadsize, sc->file);

    // always close file
    g_pFileSystem->Close(sc->file);
    sc->file = nullptr;
  }

  if (data == nullptr)
  {
    gEngfuncs.Con_DPrintf("S_LoadStreamSound: Couldn't load %s\n", s->name);
    return nullptr;
  }

  //need to parse wave info
  if (sc->length == 0)
  {
    if (!GetWavinfo(&info, s->name, (byte *)data, loadsize))
      return nullptr;

    sc->length = info.samples;
    sc->loopstart = info.loopstart;
    sc->loopend = info.loopend;
    sc->samplerate = info.rate;
    sc->width = info.width;
    sc->channels = info.channels;
    sc->dataofs = info.dataofs;
    sc->bitrate = info.bps;
    sc->blockalign = info.align; //IMPORTANT: The OpenAL Buffer Size must be an exact multiple of the BlockAlignment ...

    //Not used
    sc->datalen = 0;
  }

  // For OpenAL
  if (sc->decoder == nullptr)
  {
    char al_file_path[MAX_PATH];
    g_pFileSystem->GetLocalPath(namebuffer, al_file_path, sizeof(al_file_path));
    if (al_file_path != nullptr && al_file_path[0] != 0)
    {
      auto context = alure::Context::GetCurrent();
      try
      {
        sc->decoder = context.createDecoder(al_file_path);
      }
      catch (const std::exception& e)
      {
        gEngfuncs.Con_DPrintf("S_LoadStreamSound: %s: %s\n", namebuffer, e.what());
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

  char	namebuffer[256];
  FileHandle_t hFile;
  byte	*data;
  int		filesize;
  aud_sfxcache_t *sc;

  if (s->name[0] == '*')
    return S_LoadStreamSound(s, ch);

  if (s->name[0] == '?')
    return nullptr;// VoiceSE_GetSFXCache(s, ch);

  sc = (aud_sfxcache_t *)Cache_Check(&s->cache);
  if (sc)
    return sc;

  strcpy(namebuffer, "sound");
  if (s->name[0] != '/')
    strncat(namebuffer, "/", sizeof(namebuffer) - strlen(namebuffer) - 1);
  strncat(namebuffer, s->name, sizeof(namebuffer) - strlen(namebuffer) - 1);

  data = nullptr;
  hFile = g_pFileSystem->Open(namebuffer, "rb");

  // Normal cache
  if (hFile)
  {
    filesize = g_pFileSystem->Size(hFile);
    data = (byte *)Hunk_TempAlloc(filesize + 1);
    g_pFileSystem->Read(data, filesize, hFile);
    g_pFileSystem->Close(hFile);
    hFile = nullptr;
  }
  else
  {
    namebuffer[0] = '\0';
    if (s->name[0] != '/')
      strncat(namebuffer, "/", sizeof(namebuffer) - strlen(namebuffer) - 1);
    strncat(namebuffer, s->name, sizeof(namebuffer) - 1);
    namebuffer[sizeof(namebuffer) - 1] = 0;

    hFile = g_pFileSystem->Open(namebuffer, "rb");

    if (hFile)
    {
      filesize = g_pFileSystem->Size(hFile);
      data = (byte *)Hunk_TempAlloc(filesize + 1);
      g_pFileSystem->Read(data, filesize, hFile);
      g_pFileSystem->Close(hFile);
      hFile = nullptr;
    }
  }

  if (data == nullptr)
  {
    gEngfuncs.Con_DPrintf("S_LoadSound: Couldn't load %s\n", namebuffer);
    return nullptr;
  }

  wavinfo_t info;

  if (!GetWavinfo(&info, s->name, data, filesize))
    return nullptr;

  if (info.width > 2)
  {
    gEngfuncs.Con_DPrintf("S_LoadSound: Couldn't load %s, width > 16bits\n", namebuffer);
    return nullptr;
  }

  if (info.channels > 2)
  {
    gEngfuncs.Con_DPrintf("S_LoadSound: Couldn't load %s, channels > 2\n", namebuffer);
    return nullptr;
  }

  int datalen = info.samples * info.width * info.channels;

  sc = (aud_sfxcache_t *)Cache_Alloc(&s->cache, datalen + sizeof(aud_sfxcache_t), s->name);
  if (sc == nullptr)
    return nullptr;

  memset(sc, 0, sizeof(aud_sfxcache_t));

  //we still give it a value though we don't need it
  sc->file = nullptr;
  sc->filesize = filesize;

  sc->length = info.samples; //number of samples ( include channels )
  sc->loopstart = info.loopstart; //-1 or loop start position
  sc->loopend = info.loopend;
  sc->samplerate = info.rate; //sample rate = 11025 / 22050 / 44100
  sc->width = info.width; //bits = 8 / 16
  sc->channels = info.channels; //channels = mono(1) / stereo(2)
  sc->dataofs = info.dataofs; //the offset to the data chunk
  sc->bitrate = info.bps; //bit rate, how many bits per seconds
  sc->blockalign = info.align; //IMPORTANT: The OpenAL Buffer Size must be an exact multiple of the BlockAlignment ...

  //For VOX_ usage
  sc->datalen = datalen - (datalen % info.align);
  sc->data = std::vector<byte>(data + info.dataofs, data + info.dataofs + sc->datalen);

  // For OpenAL
  if (sc->buffer == nullptr)
  {
    char al_file_path[MAX_PATH];
    g_pFileSystem->GetLocalPath(namebuffer, al_file_path, sizeof(al_file_path));
    if (al_file_path != nullptr && al_file_path[0] != 0)
    {
      auto context = alure::Context::GetCurrent();
      try
      {
        sc->buffer = alure::MakeShared<alure::Buffer>(context.getBuffer(al_file_path));
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
      }
      catch (const std::exception& e)
      {
        gEngfuncs.Con_DPrintf("S_LoadSound: %s: %s\n", namebuffer, e.what());
      }
    }
  }

  return sc;
}