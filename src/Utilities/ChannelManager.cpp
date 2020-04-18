#include "Utilities/ChannelManager.hpp"
#include "Vox/VoxManager.hpp"

namespace MetaAudio
{
  ChannelManager::ChannelManager()
  {
    al_xfi_workaround = gEngfuncs.pfnGetCvarPointer("al_xfi_workaround");
  }

  bool ChannelManager::IsPlaying(sfx_t* sfx)
  {
    auto functor = [&](auto& channel) { return channel.sfx == sfx && channel.source && IsPlaying(channel); };

    return std::any_of(channels.dynamic.begin(), channels.dynamic.end(), functor) ||
           std::any_of(channels.static_.begin(), channels.static_.end(), functor);
  }

  bool ChannelManager::IsPlaying(const aud_channel_t& channel)
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

  void ChannelManager::FreeChannel(aud_channel_t* ch)
  {
    auto it = std::find_if(channels.static_.begin(), channels.static_.end(), [&](aud_channel_t& channel) { return &channel == ch; });
    if (it != channels.static_.end())
    {
      channels.static_.erase(it);
    }
    else
    {
      it = std::find_if(channels.dynamic.begin(), channels.dynamic.end(), [&](aud_channel_t& channel) { return &channel == ch; });
      if (it != channels.dynamic.end())
      {
        channels.dynamic.erase(it);
      }
    }
  }

  aud_channel_t* ChannelManager::SND_PickStaticChannel(int entnum, int entchannel, sfx_t* sfx)
  {
    return &channels.static_.emplace_back();
  }

  aud_channel_t* ChannelManager::SND_PickDynamicChannel(int entnum, int entchannel, sfx_t* sfx)
  {
    if (entchannel == CHAN_STREAM && IsPlaying(sfx))
    {
      return nullptr;
    }

    return &channels.dynamic.emplace_back();
  }

  void ChannelManager::ClearAllChannels()
  {
    channels.dynamic.clear();
    channels.static_.clear();
  }

  void ChannelManager::ClearEntityChannels(int entnum, int entchannel)
  {
    auto functor = [&](auto& channel) { return channel.entnum == entnum && channel.entchannel == entchannel; };

    channels.dynamic.erase(std::remove_if(channels.dynamic.begin(), channels.dynamic.end(), functor), channels.dynamic.end());
    channels.static_.erase(std::remove_if(channels.static_.begin(), channels.static_.end(), functor), channels.static_.end());
  }

  void ChannelManager::ClearFinished()
  {
    auto functor = [&](aud_channel_t& channel) { return channel.isentence < 0 && !IsPlaying(channel); };

    channels.dynamic.erase(std::remove_if(channels.dynamic.begin(), channels.dynamic.end(), functor), channels.dynamic.end());
    channels.static_.erase(std::remove_if(channels.static_.begin(), channels.static_.end(), functor), channels.static_.end());
  }

  int ChannelManager::S_AlterChannel(int entnum, int entchannel, sfx_t* sfx, float fvol, float pitch, int flags)
  {
    std::function<bool(aud_channel_t& channel)> functor;

    auto internalFunctor = [&](aud_channel_t& channel)
    {
      if (flags & SND_CHANGE_PITCH)
      {
        channel.pitch = pitch;
        channel.source.setPitch(channel.pitch);
      }

      if (flags & SND_CHANGE_VOL)
      {
        channel.volume = fvol;
        channel.source.setGain(channel.volume);
      }

      if (flags & SND_STOP)
      {
        channel.source.stop();
      }
    };

    if (sfx->name[0] == '!')
    {
      // This is a sentence name.
      // For sentences: assume that the entity is only playing one sentence
      // at a time, so we can just shut off
      // any channel that has ch->isentence >= 0 and matches the
      // soundsource.
      functor = [&](aud_channel_t& channel)
      {
        if (channel.entnum == entnum &&
            channel.entchannel == entchannel &&
            channel.sfx != nullptr &&
            channel.isentence >= 0)
        {
          internalFunctor(channel);
          return true;
        }
        else
        {
          return false;
        }
      };
    }
    else
    {
      functor = [&](aud_channel_t& channel)
      {
        if (channel.entnum == entnum &&
            channel.entchannel == entchannel &&
            channel.sfx == sfx)
        {
          internalFunctor(channel);
          return true;
        }
        else
        {
          return false;
        }
      };
    }

    return std::any_of(channels.dynamic.begin(), channels.dynamic.end(), functor) ||
           std::any_of(channels.static_.begin(), channels.static_.end(), functor);
  }
}