#pragma once
#include <memory>

#include "snd_local.h"

namespace MetaAudio
{
  class VoxManager;

  class ChannelManager final
  {
  private:
    std::weak_ptr<VoxManager> vox;
    cvar_t* al_xfi_workaround;

    struct
    {
      std::vector<aud_channel_t> static_;
      std::vector<aud_channel_t> dynamic;
    } channels;

  public:
    ChannelManager(); // TODO: add pool update, to check for finished channels and remove them

    aud_channel_t* SND_PickStaticChannel(int entnum, int entchannel, sfx_t* sfx);
    aud_channel_t* SND_PickDynamicChannel(int entnum, int entchannel, sfx_t* sfx);
    int S_AlterChannel(int entnum, int entchannel, sfx_t* sfx, float fvol, float pitch, int flags);

    bool IsPlaying(sfx_t* sfx);
    bool IsPlaying(const aud_channel_t& channel);

    void FreeChannel(aud_channel_t* ch);

    void ClearAllChannels();
    void ClearEntityChannels(int entnum, int entchannel);
    void ClearFinished();

    template<class Functor>
    void ForEachValidChannel(Functor& lambda)
    {
      std::for_each(channels.dynamic.begin(), channels.dynamic.end(), lambda);
      std::for_each(channels.static_.begin(), channels.static_.end(), lambda);
    }

    // delete copy
    ChannelManager(const ChannelManager& other) = delete;
    ChannelManager& ChannelManager::operator=(const ChannelManager& other) = delete;

    // HACK for now. Need to do a better refactor.
    void SetVox(std::shared_ptr<VoxManager> vox);
  };
}