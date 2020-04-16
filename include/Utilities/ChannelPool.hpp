#pragma once
#include <memory>

#include "snd_local.h"

namespace MetaAudio
{
  class VoxManager;

  class ChannelPool final
  {
  private:
    std::weak_ptr<VoxManager> vox;
    cvar_t* al_xfi_workaround;

    alure::Array<aud_channel_t, MAX_CHANNELS> channels{};
    size_t total_channels;

    bool ChannelCheckIsPlaying(const aud_channel_t& channel);

  public:
    ChannelPool();

    aud_channel_t* SND_PickStaticChannel(int entnum, int entchannel, sfx_t* sfx);
    aud_channel_t* SND_PickDynamicChannel(int entnum, int entchannel, sfx_t* sfx);
    int S_AlterChannel(int entnum, int entchannel, sfx_t* sfx, float fvol, float pitch, int flags);

    bool IsPlaying(sfx_t* sfx);
    bool IsPlaying(const aud_channel_t& channel);

    void FreeChannel(aud_channel_t* ch);

    void ClearAllChannels();
    void ClearEntityChannels(int entnum, int entchannel);

    template<class Functor>
    void ForEachValidChannel(bool includeAmbients, Functor& lambda)
    {
      for (size_t i = includeAmbients ? 0 : NUM_AMBIENTS; i < total_channels; ++i)
      {
        lambda(channels[i]);
      }
    }

    // HACK for now. Need to do a better refactor.
    void SetVox(std::shared_ptr<VoxManager> vox);
  };
}