#include "Utilities/ChannelPool.hpp"
#include "Vox/VoxManager.hpp"

namespace MetaAudio
{
  ChannelPool::ChannelPool() : total_channels(0)
  {
    al_xfi_workaround = gEngfuncs.pfnGetCvarPointer("al_xfi_workaround");
  }

  void ChannelPool::SetVox(std::shared_ptr<VoxManager> vox)
  {
    this->vox = vox;
  }

  bool ChannelPool::IsPlaying(sfx_t* sfx)
  {
    for (auto& channel : channels)
    {
      if (channel.sfx == sfx && channel.source && ChannelCheckIsPlaying(channel))
      {
        return true;
      }
    }

    return false;
  }

  bool ChannelPool::IsPlaying(const aud_channel_t& channel)
  {
    return ChannelCheckIsPlaying(channel);
  }

  bool ChannelPool::ChannelCheckIsPlaying(const aud_channel_t& channel)
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

  void ChannelPool::FreeChannel(aud_channel_t* ch)
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
        vox.lock()->rgrgvoxword[ch->isentence][i].sfx = nullptr;
      }
    }

    ch->isentence = -1;
    ch->sfx = nullptr;

    vox.lock()->CloseMouth(ch);
  }

  aud_channel_t* ChannelPool::SND_PickStaticChannel(int entnum, int entchannel, sfx_t* sfx)
  {
    size_t i;
    aud_channel_t* ch = nullptr;

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

  aud_channel_t* ChannelPool::SND_PickDynamicChannel(int entnum, int entchannel, sfx_t* sfx)
  {
    int ch_idx;
    int first_to_die;

    if (entchannel == CHAN_STREAM && IsPlaying(sfx))
      return nullptr;

    first_to_die = -1;

    float life_left = 99999;
    float life;
    uint64_t played;

    aud_channel_t* ch;

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

      aud_sfxcache_t* sc = static_cast<aud_sfxcache_t*>(sfx->cache.data);
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
      FreeChannel(&(channels[first_to_die]));
    }

    return &channels[first_to_die];
  }

  void ChannelPool::ClearAllChannels()
  {
    for (auto& channel : channels)
    {
      if (channel.sfx != nullptr)
      {
        FreeChannel(&channel);
      }
    }

    channels.fill(aud_channel_t());
    total_channels = MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS;
  }

  void ChannelPool::ClearEntityChannels(int entnum, int entchannel)
  {
    for (size_t i = NUM_AMBIENTS; i < total_channels; ++i)
    {
      if (channels[i].entnum == entnum && channels[i].entchannel == entchannel)
      {
        FreeChannel(&channels[i]);
      }
    }
  }

  int ChannelPool::S_AlterChannel(int entnum, int entchannel, sfx_t* sfx, float fvol, float pitch, int flags)
  {
    size_t ch_idx;
    aud_channel_t* ch;

    if (sfx->name[0] == '!')
    {
      // This is a sentence name.
      // For sentences: assume that the entity is only playing one sentence
      // at a time, so we can just shut off
      // any channel that has ch->isentence >= 0 and matches the
      // soundsource.

      for (ch_idx = 0; ch_idx < total_channels; ++ch_idx)
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
            FreeChannel(ch);
          }

          return true;
        }
      }
      // channel not found
      return false;
    }

    for (ch_idx = 0; ch_idx < total_channels; ++ch_idx)
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
          FreeChannel(ch);
        }

        return true;
      }
    }

    return false;
  }
}