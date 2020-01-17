#include <metahook.h>

#include "snd_local.h"
#include "snd_fx.hpp"
#include "snd_voice.hpp"
#include "snd_vox.hpp"
#include "snd_loader.hpp"
#include "zone.h"

//sfx struct
std::unordered_map<alure::String, sfx_t> known_sfx;

//channels
alure::Array<aud_channel_t, MAX_CHANNELS> channels{};
int total_channels;

//engine cvars
cvar_t *nosound = nullptr;
cvar_t *volume = nullptr;
cvar_t *sxroom_off = nullptr;
cvar_t *sxroomwater_type = nullptr;
cvar_t *sxroom_type = nullptr;
cvar_t *snd_show = nullptr;

//active control
cvar_t *al_doppler = nullptr;
cvar_t *al_xfi_workaround = nullptr;
qboolean openal_started = false;
qboolean openal_mute = false;

//other cvars

//OpenAL device
static alure::DeviceManager al_dev_manager;
static alure::Device al_device;
static alure::Context al_context;
static alure::UniquePtr<EnvEffects> al_efx;
static alure::UniquePtr<VOX> vox;
char al_device_name[1024] = "";
int al_device_majorversion = 0;
int al_device_minorversion = 0;

//Print buffer
std::string dprint_buffer;

// translates from AL coordinate system to quake
// HL seems to use inches, convert to meters.
#define AL_UnitToMeters 0.0254f
#define AL_UnpackVector(v) -v[1] * AL_UnitToMeters, v[2] * AL_UnitToMeters, -v[0] * AL_UnitToMeters
#define AL_CopyVector(a, b) ((b)[0] = -(a)[1], (b)[1] = (a)[2], (b)[2] = -(a)[0])
#define VectorCopy(a, b) {(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];}

static bool ChannelCheckIsPlaying(const aud_channel_t& channel)
{
  if (channel.source)
  {
    if (al_xfi_workaround->value == 0.0f ||
      al_xfi_workaround->value == 2.0f ||
      channel.source.getLooping() ||
      channel.entchannel == CHAN_STREAM ||
      (channel.entchannel >= CHAN_NETWORKVOICE_BASE && channel.entchannel <= CHAN_NETWORKVOICE_END) ||
      channel.decoder != nullptr ||
      channel.buffer == nullptr)
    {
      return channel.source.isPlaying();
    }
    else
    {
      return channel.source.isPlaying() && std::chrono::steady_clock::now() < channel.playback_end_time;
    }
  }
  return false;
}

void S_FreeCache(sfx_t *sfx)
{
  aud_sfxcache_t *sc = static_cast<aud_sfxcache_t *>(sfx->cache.data);
  if (!sc)
    return;

  //2015-12-12 fixed a bug that a buffer in use is freed
  if (SND_IsPlaying(sfx))
    return;

  if (sc->buffer)
  {
    al_context.removeBuffer(sc->buffer);
  }

  Cache_Free(sfx->name);

  sfx->cache.data = nullptr;
}

void S_FlushCaches(void)
{
  for (auto& sfx : known_sfx)
  {
    S_FreeCache(&(sfx.second));
  }
}

sfx_t *S_FindName(char *name, int *pfInCache)
{
  try
  {
    sfx_t *sfx = nullptr;

    if (!name)
      Sys_ErrorEx("S_FindName: NULL\n");

    if (strlen(name) >= MAX_QPATH)
      Sys_ErrorEx("Sound name too long: %s", name);

    auto sfx_iterator = known_sfx.find(name);
    if (sfx_iterator != known_sfx.end())
    {
      if (pfInCache)
      {
        *pfInCache = sfx_iterator->second.cache.data != nullptr ? 1 : 0;
      }

      if (sfx_iterator->second.servercount > 0)
        sfx_iterator->second.servercount = *gAudEngine.cl_servercount;

      return &(sfx_iterator->second);
    }
    else
    {
      for (auto& sfxElement : known_sfx)
      {
        if (sfxElement.second.servercount > 0 && sfxElement.second.servercount != *gAudEngine.cl_servercount)
        {
          S_FreeCache(&(sfxElement.second));
          known_sfx.erase(sfxElement.first);
          break;
        }
      }
    }

    if (!sfx)
    {
      auto result = known_sfx.emplace(name, sfx_t());
      sfx = &(result.first->second);
    }
    else
    {
      //free OpenAL buffer and cache
      S_FreeCache(sfx);
    }

    strncpy_s(sfx->name, name, sizeof(sfx->name) - 1);
    sfx->name[sizeof(sfx->name) - 1] = 0;

    if (pfInCache)
      *pfInCache = 0;

    sfx->servercount = *gAudEngine.cl_servercount;
    return sfx;
  }
  catch (const std::exception& e)
  {
    MessageBox(NULL, e.what(), "Error on S_FindName", MB_ICONERROR);
    exit(0);
  }
}

void S_CheckWavEnd(aud_channel_t *ch, aud_sfxcache_t *sc)
{
  if (ch->voicecache)
  {
    return;
  }

  if (!sc)
    return;

  qboolean fWaveEnd = false;

  if (!ChannelCheckIsPlaying(*ch))
  {
    fWaveEnd = true;
  }
  else if (ch->entchannel != CHAN_STREAM)
  {
    uint64_t iSamplesPlayed = ch->source.getSampleOffset();

    if (!sc->looping && iSamplesPlayed >= ch->end)
    {
      fWaveEnd = true;
      ch->source.stop();
    }
  }

  if (!fWaveEnd)
    return;

  if (ch->isentence >= 0)
  {
    sfx_t *sfx;

    if (vox->rgrgvoxword[ch->isentence][ch->iword].sfx && !vox->rgrgvoxword[ch->isentence][ch->iword].fKeepCached)
      S_FreeCache(vox->rgrgvoxword[ch->isentence][ch->iword].sfx);

    ch->sfx = sfx = vox->rgrgvoxword[ch->isentence][ch->iword + 1].sfx;

    if (sfx)
    {
      sc = S_LoadSound(sfx, ch);

      if (sc)
      {
        ch->start = 0;
        ch->end = sc->length;
        ch->iword++;

        vox->TrimStartEndTimes(ch, sc);
        if (ch->entchannel == CHAN_STREAM)
        {
          ch->decoder = sc->decoder;
          ch->source.setOffset(ch->start);
          ch->source.play(ch->decoder, 12000, 4);
        }
        else
        {
          ch->buffer = sc->buffer;
          ch->source.setOffset(ch->start);
          ch->playback_end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(static_cast<long long>(static_cast<double>(ch->buffer.getLength()) / ch->buffer.getFrequency() * 1.5 * 1000)); // 50% of safety
          ch->source.play(ch->buffer);
        }

        return;
      }
    }
  }

  // Free the channel up if source has stopped and there is nothing else to do
  S_FreeChannel(ch);
}

void SND_Spatialize(aud_channel_t *ch, qboolean init)
{
  ch->firstpass = init;
  if (!ch->sfx)
    return;

  // invalid source
  if (!ch->source)
    return;

  //apply effect
  qboolean underwater = (*gAudEngine.cl_waterlevel > 2) ? true : false;
  al_efx->ApplyEffect(ch, underwater);

  //for later usage
  aud_sfxcache_t *sc = static_cast<aud_sfxcache_t *>(ch->sfx->cache.data);

  //move mouth
  if (ch->entnum > 0 && (ch->entchannel == CHAN_VOICE || ch->entchannel == CHAN_STREAM))
  {
    if (sc && sc->channels == alure::ChannelConfig::Mono && !sc->data.empty())
    {
      vox->MoveMouth(ch, sc);
    }
  }

  //update position
  alure::Vector3 alure_position(0, 0, 0);
  if (ch->entnum != *gAudEngine.cl_viewentity)
  {
    ch->source.setRelative(false);
    if (ch->entnum > 0 && ch->entnum < *gAudEngine.cl_num_entities)
    {
      cl_entity_t *sent = gEngfuncs.GetEntityByIndex(ch->entnum);

      if (sent && sent->model && sent->curstate.messagenum == *gAudEngine.cl_parsecount)
      {
        VectorCopy(sent->origin, ch->origin);

        if (sent->model->type == mod_brush)
        {
          // Mobile brushes (such as trains and platforms) have the correct origin set,
          // but most other bushes do not. How to correctly detect them?
          if (sent->baseline.origin[0] == 0.0f || sent->baseline.origin[1] == 0.0f || sent->baseline.origin[2] == 0.0f)
          {
            ch->origin[0] = (sent->curstate.mins[0] + sent->curstate.maxs[0]) * 0.5f + sent->curstate.origin[0];
            ch->origin[1] = (sent->curstate.mins[1] + sent->curstate.maxs[1]) * 0.5f + sent->curstate.origin[1];
            ch->origin[2] = (sent->curstate.mins[2] + sent->curstate.maxs[2]) * 0.5f + sent->curstate.origin[2];
          }
        }

        float ratio = 0;
        if ((*gAudEngine.cl_time) != (*gAudEngine.cl_oldtime))
        {
          ratio = static_cast<float>(1 / ((*gAudEngine.cl_time) - (*gAudEngine.cl_oldtime)));
        }

        vec3_t sent_velocity = { (sent->curstate.origin[0] - sent->prevstate.origin[0]) * ratio,
          (sent->curstate.origin[1] - sent->prevstate.origin[1]) * ratio,
          (sent->curstate.origin[2] - sent->prevstate.origin[2]) * ratio };

        ch->source.setVelocity({ AL_UnpackVector(sent_velocity) });
        ch->source.setRadius(sent->model->radius * AL_UnitToMeters);
      }
    }
    else
    {
      // It seems that not only sounds from the view entity can be source relative...
      if (ch->origin[0] == 0.0f && ch->origin[1] == 0.0f && ch->origin[2] == 0.0f)
      {
        ch->source.setRelative(true);
      }
    }
    alure_position = { AL_UnpackVector(ch->origin) };
  }
  else
  {
    ch->source.setRelative(true);
  }
  ch->source.setPosition(alure_position);

  float fvol = 1.0f;
  float fpitch = 1.0f;

  vox->SetChanVolPitch(ch, &fvol, &fpitch);

  if (sc && sc->length != 0x40000000)
  {
    fvol *= (*gAudEngine.g_SND_VoiceOverdrive);
  }

  ch->source.setGain(ch->volume * fvol);
  ch->source.setPitch(ch->pitch * fpitch);

  if (!init)
  {
    S_CheckWavEnd(ch, sc);
  }
}

void S_Update(float *origin, float *forward, float *right, float *up)
{
  try
  {
    int i, total;
    vec_t orientation[6];

    // Update Alure's OpenAL context at the start of processing.
    al_context.update();

    // Print buffer and clear it.
    if (dprint_buffer.length())
    {
      gEngfuncs.Con_DPrintf(const_cast<char *>((dprint_buffer.c_str())));
      dprint_buffer.clear();
    }

    AL_CopyVector(forward, orientation);
    AL_CopyVector(up, orientation + 3);

    alure::Listener al_listener = al_context.getListener();
    if (openal_mute)
    {
      al_listener.setGain(0.0f);
    }
    else
    {
      if (volume)
        al_listener.setGain(std::clamp(volume->value, 0.0f, 1.0f));
      else
        al_listener.setGain(1.0f);
    }

    al_context.setDopplerFactor(std::clamp(al_doppler->value, 0.0f, 10.0f));

    std::pair<alure::Vector3, alure::Vector3> alure_orientation(
      alure::Vector3(orientation[0], orientation[1], orientation[2]),
      alure::Vector3(orientation[3], orientation[4], orientation[5])
    );

    // Force unit vector if all zeros (Rapture3D workaround).
    if (orientation[0] == 0.0f && orientation[1] == 0.0f && orientation[2] == 0.0f)
    {
      alure_orientation.first[0] = 1;
    }
    if (orientation[3] == 0.0f && orientation[4] == 0.0f && orientation[5] == 0.0f)
    {
      alure_orientation.second[0] = 1;
    }

    cl_entity_t *pent = gEngfuncs.GetEntityByIndex(*gAudEngine.cl_viewentity);
    if (pent != nullptr)
    {
      float ratio = 0;
      if ((*gAudEngine.cl_time) != (*gAudEngine.cl_oldtime))
      {
        ratio = static_cast<float>(1 / ((*gAudEngine.cl_time) - (*gAudEngine.cl_oldtime)));
      }

      vec3_t view_velocity = { (pent->curstate.origin[0] - pent->prevstate.origin[0]) * ratio,
        (pent->curstate.origin[1] - pent->prevstate.origin[1]) * ratio,
        (pent->curstate.origin[2] - pent->prevstate.origin[2]) * ratio };

      al_listener.setVelocity({ AL_UnpackVector(view_velocity) });
    }
    al_listener.setPosition({ AL_UnpackVector(origin) });
    al_listener.setOrientation(alure_orientation);

    int roomtype = 0;
    bool underwater = (*gAudEngine.cl_waterlevel > 2) ? true : false;
    if (sxroomwater_type && sxroom_type)
    {
      roomtype = underwater ? (int)sxroomwater_type->value : (int)sxroom_type->value;
    }
    al_efx->InterplEffect(roomtype);

    for (i = NUM_AMBIENTS; i < total_channels; i++)
    {
      SND_Spatialize(&channels[i], false);
    }

    if (snd_show && snd_show->value)
    {
      std::string output;
      total = 0;
      for (i = 0; i < total_channels; i++)
      {
        if (channels[i].sfx && channels[i].volume > 0)
        {
          output.append(std::to_string(static_cast<int>(channels[i].volume * 255.0f)) + " " + channels[i].sfx->name + "\n");
          total++;
        }
      }

      output.append("----(" + std::to_string(total) + ")----\n");
      gEngfuncs.Con_Printf(const_cast<char *>(output.c_str()));
    }
  }
  catch (const std::exception& e)
  {
    MessageBox(NULL, e.what(), "Error on S_Update", MB_ICONERROR);
    exit(0);
  }
}

void S_FreeChannel(aud_channel_t *ch)
{
  if (ch->source)
  {
    // Stop the Source and reset buffer
    ch->buffer = nullptr;
    ch->source.stop();
    ch->source.destroy();
  }

  if (ch->decoder)
  {
    ch->decoder.reset();
  }

  if (ch->isentence >= 0)
  {
    for (size_t i = 0; i < CVOXWORDMAX; ++i)
    {
      vox->rgrgvoxword[ch->isentence][i].sfx = nullptr;
    }
  }

  ch->isentence = -1;
  ch->sfx = nullptr;

  vox->CloseMouth(ch);
}

int S_AlterChannel(int entnum, int entchannel, sfx_t *sfx, float fvol, float pitch, int flags)
{
  int ch_idx;
  aud_channel_t *ch;

  if (sfx->name[0] == '!')
  {
    // This is a sentence name.
    // For sentences: assume that the entity is only playing one sentence
    // at a time, so we can just shut off
    // any channel that has ch->isentence >= 0 and matches the
    // soundsource.

    for (ch_idx = 0; ch_idx < total_channels; ch_idx++)
    {
      ch = &channels[ch_idx];
      if (ch->entnum == entnum
        && ch->entchannel == entchannel
        && ch->sfx != nullptr
        && ch->isentence >= 0)
      {
        if (flags & SND_CHANGE_PITCH)
        {
          ch->pitch = pitch;
          ch->source.setPitch(ch->pitch);
        }

        if (flags & SND_CHANGE_VOL)
        {
          ch->volume = fvol;
          ch->source.setGain(ch->volume);
        }

        if (flags & SND_STOP)
        {
          S_FreeChannel(ch);
        }

        return true;
      }
    }
    // channel not found
    return false;
  }

  for (ch_idx = 0; ch_idx < total_channels; ch_idx++)
  {
    ch = &channels[ch_idx];
    if (ch->entnum == entnum
      && ch->entchannel == entchannel
      && ch->sfx == sfx)
    {
      if (flags & SND_CHANGE_PITCH)
      {
        ch->pitch = pitch;
        ch->source.setPitch(ch->pitch);
      }

      if (flags & SND_CHANGE_VOL)
      {
        ch->volume = fvol;
        ch->source.setGain(ch->volume);
      }

      if (flags & SND_STOP)
      {
        S_FreeChannel(ch);
      }

      return true;
    }
  }

  return false;
}

bool SND_IsPlaying(sfx_t *sfx)
{
  int ch_idx;

  for (ch_idx = 0; ch_idx < MAX_CHANNELS; ch_idx++)
  {
    if (channels[ch_idx].sfx == sfx && channels[ch_idx].source && ChannelCheckIsPlaying(channels[ch_idx]))
    {
      return true;
    }
  }

  return false;
}

aud_channel_t *SND_PickDynamicChannel(int entnum, int entchannel, sfx_t *sfx)
{
  int ch_idx;
  int first_to_die;

  if (entchannel == CHAN_STREAM && SND_IsPlaying(sfx))
    return nullptr;

  first_to_die = -1;

  float life_left = 99999;
  float life;
  uint64_t played;

  aud_channel_t *ch;

  // Check all channels and check if it is available for use
  for (ch_idx = NUM_AMBIENTS; ch_idx < NUM_AMBIENTS + MAX_DYNAMIC_CHANNELS; ch_idx++)
  {
    ch = &channels[ch_idx];
    if (ch->entchannel == CHAN_STREAM && channels[ch_idx].source && ChannelCheckIsPlaying(*ch))
    {
      if (entchannel == CHAN_VOICE)
        return nullptr;

      continue;
    }

    // Appointed channel
    if (entchannel != 0 && ch->entnum == entnum && (ch->entchannel == entchannel || entchannel == -1))
    {
      first_to_die = ch_idx;
      break;
    }

    if (ch->entnum == *gAudEngine.cl_viewentity && entnum != *gAudEngine.cl_viewentity && ch->sfx)
      continue;

    if (!ch->source)
    {
      first_to_die = ch_idx;
      break;
    }

    if (ch->sfx == nullptr)
    {
      first_to_die = ch_idx;
      break;
    }

    aud_sfxcache_t *sc = static_cast<aud_sfxcache_t *>(sfx->cache.data);
    if (sc == nullptr)
    {
      first_to_die = ch_idx;
      break;
    }

    if (!ChannelCheckIsPlaying(*ch))
    {
      first_to_die = ch_idx;
      break;
    }

    played = ch->source.getSampleOffset();
    if (ch->decoder)
    {
      life = static_cast<float>(ch->end - played) / ch->decoder->getFrequency();
    }
    else
    {
      life = static_cast<float>(ch->end - played) / ch->buffer.getFrequency();
    }

    if (life < life_left)
    {
      life_left = life;
      first_to_die = ch_idx;
    }
  }

  if (first_to_die == -1)
    return nullptr;

  if (channels[first_to_die].sfx)
  {
    S_FreeChannel(&(channels[first_to_die]));
  }

  return &channels[first_to_die];
}

aud_channel_t *SND_PickStaticChannel(int entnum, int entchannel, sfx_t *sfx)
{
  int i;
  aud_channel_t *ch = nullptr;

  for (i = MAX_DYNAMIC_CHANNELS; i < total_channels; i++)
  {
    if (channels[i].sfx == nullptr)
      break;

    // This should allow channels to be reused, but won't work on some
    // bugged Creative drivers.
    if (channels[i].source)
    {
      if (!ChannelCheckIsPlaying(channels[i]))
      {
        break;
      }
    }
  }

  if (i < total_channels)
  {
    ch = &channels[i];
  }
  else
  {
    // no empty slots, alloc a new static sound channel
    if (total_channels == MAX_CHANNELS)
    {
      gEngfuncs.Con_DPrintf("total_channels == MAX_CHANNELS\n");
      return nullptr;
    }

    // get a channel for the static sound
    ch = &channels[total_channels];
    total_channels++;
  }

  return ch;
}

void S_StartSound(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch, bool is_static)
{
  std::string _function_name;
  if (is_static)
  {
    _function_name = "S_StartStaticSound";
  }
  else
  {
    _function_name = "S_StartDynamicSound";
  }

  aud_channel_t* ch;
  aud_sfxcache_t* sc;
  float fpitch;

  if (!sfx)
    return;

  if (nosound && nosound->value)
    return;

  if (strncmp(sfx->name, "common/null.wav", 16) == 0)
    return;

  if (sfx->name[0] == '*')
    entchannel = CHAN_STREAM;

  if (entchannel == CHAN_STREAM && pitch != 100)
  {
    gEngfuncs.Con_DPrintf("Warning: pitch shift ignored on stream sound %s\n", sfx->name);
    pitch = 100;
  }

  if (fvol > 1.0f)
  {
    gEngfuncs.Con_DPrintf("%s: %s fvolume > 1.0", _function_name, sfx->name);
  }

  fpitch = pitch / 100.0f;

  if (flags & (SND_STOP | SND_CHANGE_VOL | SND_CHANGE_PITCH))
  {
    if (S_AlterChannel(entnum, entchannel, sfx, fvol, fpitch, flags))
      return;

    if (flags & SND_STOP)
      return;
  }

  if (pitch == 0)
  {
    gEngfuncs.Con_DPrintf("Warning: %s Ignored, called with pitch 0", _function_name);
    return;
  }

  if (is_static)
  {
    ch = SND_PickStaticChannel(entnum, entchannel, sfx);
  }
  else
  {
    ch = SND_PickDynamicChannel(entnum, entchannel, sfx);
  }

  if (!ch)
  {
    return;
  }

  VectorCopy(origin, ch->origin);
  ch->attenuation = attenuation;
  ch->volume = fvol;
  ch->entnum = entnum;
  ch->entchannel = entchannel;
  ch->pitch = fpitch;
  ch->isentence = -1;

  if (sfx->name[0] == '!' || sfx->name[0] == '#')
  {
    sc = vox->LoadSound(ch, sfx->name + 1);
  }
  else
  {
    sc = S_LoadSound(sfx, ch);
    ch->sfx = sfx;
  }

  if (!sc)
  {
    ch->sfx = nullptr;
    return;
  }

  if (!ch->source)
  {
    ch->source = al_context.createSource();
  }

  ch->start = 0;
  ch->end = sc->length;

  vox->TrimStartEndTimes(ch, sc);

  if (!is_static)
  {
    vox->InitMouth(entnum, entchannel);
  }

  ch->source.setPitch(ch->pitch);
  ch->source.setRolloffFactors(ch->attenuation, ch->attenuation);
  ch->source.setDistanceRange(0.0f, 1000.0f * AL_UnitToMeters);
  ch->source.setAirAbsorptionFactor(1.0f);

  // Should also set source priority
  if (ch->entchannel == CHAN_STREAM || (ch->entchannel >= CHAN_NETWORKVOICE_BASE && ch->entchannel <= CHAN_NETWORKVOICE_END))
  {
    SND_Spatialize(ch, true);
    try
    {
      ch->source.setOffset(ch->start);
      ch->decoder = sc->decoder;
      if (ch->entchannel >= CHAN_NETWORKVOICE_BASE && ch->entchannel <= CHAN_NETWORKVOICE_END)
      {
        ch->source.play(ch->decoder, 1024, 3);
        delete sc; // must be deleted here as voice data does not go to the cache to be deleted later
      }
      else
      {
        ch->source.play(ch->decoder, 4096, 4);
      }
    }
    catch (const std::runtime_error& error)
    {
      dprint_buffer.append(_function_name).append(": ").append(error.what()).append("\n");
    }
  }
  else
  {
    bool force_stream = false;
    if (al_xfi_workaround->value == 2.0f)
    {
      force_stream = true;
    }
    if (sc->looping)
    {
      ch->source.setLooping(true);
      if (sc->loopstart > 0)
      {
        auto points = sc->buffer.getLoopPoints();
        if (points.first != sc->loopstart)
        {
          // Use Alure2 stream looping facilities
          force_stream = true;
        }
      }
    }
    if (force_stream)
    {
      ch->decoder = al_context.createDecoder(sc->buffer.getName());
      SND_Spatialize(ch, true);
      try
      {
        ch->source.setOffset(ch->start);
        ch->source.play(ch->decoder, 12000, 4);
      }
      catch (const std::runtime_error& error)
      {
        dprint_buffer.append(_function_name).append(": ").append(error.what()).append("\n");
      }
    }
    else
    {
      ch->buffer = sc->buffer;
      SND_Spatialize(ch, true);
      try
      {
        ch->source.setOffset(ch->start);
        ch->playback_end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(static_cast<long long>(static_cast<double>(ch->buffer.getLength()) / ch->buffer.getFrequency() * 1.5 * 1000)); // 50% of safety
        ch->source.play(ch->buffer);
      }
      catch (const std::runtime_error& error)
      {
        dprint_buffer.append(_function_name).append(": ").append(error.what()).append("\n");
      }
    }
  }
}

void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch)
{
  try
  {
    S_StartSound(entnum, entchannel, sfx, origin, fvol, attenuation, flags, pitch, false);
  }
  catch (const std::exception& e)
  {
    MessageBox(NULL, e.what(), "Error on S_StartDynamicSound", MB_ICONERROR);
    exit(0);
  }
}

void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch)
{
  try
  {
    S_StartSound(entnum, entchannel, sfx, origin, fvol, attenuation, flags, pitch, true);
  }
  catch (const std::exception& e)
  {
    MessageBox(NULL, e.what(), "Error on S_StartStaticSound", MB_ICONERROR);
    exit(0);
  }
}

void S_StopSound(int entnum, int entchannel)
{
  try
  {
    for (int i = NUM_AMBIENTS; i < total_channels; ++i)
    {
      if (channels[i].entnum == entnum && channels[i].entchannel == entchannel)
      {
        S_FreeChannel(&channels[i]);
      }
    }
  }
  catch (const std::exception& e)
  {
    MessageBox(NULL, e.what(), "Error on S_StopSound", MB_ICONERROR);
    exit(0);
  }
}

void S_StopAllSounds(qboolean clear)
{
  try
  {
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
      if (channels[i].sfx)
      {
        S_FreeChannel(&channels[i]);
      }
    }

    channels.fill(aud_channel_t());
    total_channels = MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS;
  }
  catch (const std::exception& e)
  {
    MessageBox(NULL, e.what(), "Error on S_StopAllSounds", MB_ICONERROR);
    exit(0);
  }
}

qboolean OpenAL_Init(void)
{
  try
  {
    alure::FileIOFactory::set(alure::MakeUnique<GoldSrcFileFactory>());

    al_dev_manager = alure::DeviceManager::getInstance();

    const char *_al_set_device;
    const char *device_set = CommandLine()->CheckParm("-al_device", &_al_set_device);

    if (_al_set_device != nullptr)
      al_device = al_dev_manager.openPlayback(_al_set_device, std::nothrow);

    if (!al_device)
    {
      auto default_device = al_dev_manager.defaultDeviceName(alure::DefaultDeviceType::Full);
      al_device = al_dev_manager.openPlayback(default_device);
    }

    strncpy_s(al_device_name, al_device.getName().c_str(), sizeof(al_device_name));

    al_context = al_device.createContext();

    alure::Version ver = al_device.getALCVersion();
    al_device_majorversion = ver.getMajor();
    al_device_minorversion = ver.getMinor();

    alure::Context::MakeCurrent(al_context);
    al_context.setDistanceModel(alure::DistanceModel::Linear);
    al_efx = alure::MakeUnique<EnvEffects>(al_context, al_device.getMaxAuxiliarySends());
    return true;
  }
  catch (const std::exception& e)
  {
    const size_t size = 4096;
    char ar[size] = "Unable to load. Reason:\n";
    int zero_index = 0;
    for (int i = 0; i < size; ++i)
    {
      if (ar[i] == 0)
      {
        zero_index = i;
        break;
      }
    }

    for (int i = 0; i < size - zero_index; ++i)
    {
      if (e.what()[i] == 0 || i == size - zero_index - 1)
      {
        ar[i + zero_index] = '\0';
        break;
      }

      ar[i + zero_index] = e.what()[i];
    }
    MessageBox(NULL, ar, "OpenAL plugin error", MB_ICONERROR);
    return false;
  }
}

void S_Startup(void)
{
  gAudEngine.S_Startup();

  //stop mute me first
  openal_mute = false;

  if (!openal_started)
  {
    if (OpenAL_Init())
    {
      openal_started = true;
    }
  }
}

void AL_Version_f(void)
{
  if (openal_started)
    gEngfuncs.Con_Printf("%s\n OpenAL Device: %s\n OpenAL Version: %d.%d\n", META_AUDIO_VERSION, al_device_name, al_device_majorversion, al_device_minorversion);
  else
    gEngfuncs.Con_Printf("%s\n Failed to initalize OpenAL device.\n", META_AUDIO_VERSION, al_device_name, al_device_majorversion, al_device_minorversion);
}

void AL_ResetEFX(void)
{
  al_efx.reset();
  al_efx = alure::MakeUnique<EnvEffects>(al_context, al_device.getMaxAuxiliarySends());
}

void AL_Devices_f(bool basic = true)
{
  alure::Vector<alure::String> devices;
  if (basic)
  {
    devices = al_dev_manager.enumerate(alure::DeviceEnumeration::Basic);
  }
  else
  {
    devices = al_dev_manager.enumerate(alure::DeviceEnumeration::Full);
  }
  gEngfuncs.Con_Printf("Available OpenAL devices:\n");
  for (const alure::String& device : devices)
  {
    gEngfuncs.Con_Printf("  %s\n", device.c_str());
  }
}

void AL_DevicesBasic_f(void)
{
  AL_Devices_f(true);
}

void AL_DevicesFull_f(void)
{
  AL_Devices_f(false);
}

void S_Init(void)
{
  al_doppler = gEngfuncs.pfnRegisterVariable("al_doppler", "1", FCVAR_EXTDLL);
  al_xfi_workaround = gEngfuncs.pfnRegisterVariable("al_xfi_workaround", "0", FCVAR_EXTDLL);
  gEngfuncs.pfnAddCommand("al_version", AL_Version_f);
  gEngfuncs.pfnAddCommand("al_reset_efx", AL_ResetEFX);
  gEngfuncs.pfnAddCommand("al_show_basic_devices", AL_DevicesBasic_f);
  gEngfuncs.pfnAddCommand("al_show_full_devices", AL_DevicesFull_f);

  gAudEngine.S_Init();

  vox = alure::MakeUnique<VOX>();

  if (!gEngfuncs.CheckParm("-nosound", NULL))
  {
    nosound = gEngfuncs.pfnGetCvarPointer("nosound");
    volume = gEngfuncs.pfnGetCvarPointer("volume");
    sxroom_off = gEngfuncs.pfnGetCvarPointer("room_off");
    sxroomwater_type = gEngfuncs.pfnGetCvarPointer("waterroom_type");
    sxroom_type = gEngfuncs.pfnGetCvarPointer("room_type");
    snd_show = gEngfuncs.pfnGetCvarPointer("snd_show");

    S_StopAllSounds(true);
  }
}

void OpenAL_Shutdown(void)
{
  // Should also clear all buffers and sources.
  alure::FileIOFactory::set(nullptr);
  alure::Context::MakeCurrent(nullptr);
  al_context.destroy();

  al_device.close();
}

void S_ShutdownAL(void)
{
  if (openal_started)
  {
    S_StopAllSounds(true);
    al_efx.reset();
    vox.reset();
    OpenAL_Shutdown();
    openal_started = false;
  }
}

void S_Shutdown(void)
{
  //shall we mute OpenAL sound when S_Shutdown?
  openal_mute = true;

  gAudEngine.S_Shutdown();
}