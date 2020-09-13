#pragma once
#include <memory>

#include "snd_local.h"

namespace MetaAudio
{
  class VoxManager;

  class ChannelManager final
  {
    friend AudioEngine;

  private:
    struct
    {
      std::vector<aud_channel_t> static_;
      std::vector<aud_channel_t> dynamic;
    } channels;

    void FreeChannel(aud_channel_t* ch);

  public:
    ChannelManager() = default;

    aud_channel_t* SND_PickStaticChannel(int entnum, int entchannel, sfx_t* sfx);
    aud_channel_t* SND_PickDynamicChannel(int entnum, int entchannel, sfx_t* sfx);
    int S_AlterChannel(int entnum, int entchannel, sfx_t* sfx, float fvol, float pitch, int flags);

    bool IsPlaying(sfx_t* sfx);

    void ClearAllChannels();
    void ClearEntityChannels(int entnum, int entchannel);
    void ClearFinished();

    template<class Functor>
    void ForEachChannel(Functor& lambda)
    {
      for (auto& channel : channels.dynamic) lambda(channel);
      for (auto& channel : channels.static_) lambda(channel);
    }

    // delete copy
    ChannelManager(const ChannelManager& other) = delete;
    ChannelManager& ChannelManager::operator=(const ChannelManager& other) = delete;
  };
}