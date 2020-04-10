#pragma once
#include <unordered_map>

#include "snd_local.h"
#include "alure2.h"
#include "Effects/EnvEffects.hpp"
#include "Vox/VoxManager.hpp"
#include "Utilities/AudioCache.hpp"
#include "Loaders/SoundLoader.hpp"

namespace MetaAudio
{
  class AudioEngine final
  {
    friend class VoxManager;

  private:
    std::unordered_map<alure::String, sfx_t> known_sfx;

    //channels
    alure::Array<aud_channel_t, MAX_CHANNELS> channels{};
    int total_channels;

    //engine cvars
    cvar_t* nosound = nullptr;
    cvar_t* volume = nullptr;
    cvar_t* sxroomwater_type = nullptr;
    cvar_t* sxroom_type = nullptr;
    cvar_t* snd_show = nullptr;

    //active control
    cvar_t* al_doppler = nullptr;
    cvar_t* al_xfi_workaround = nullptr;
    bool openal_started = false;
    bool openal_mute = false;

    alure::DeviceManager al_dev_manager;
    alure::Device al_device;
    alure::Context al_context;
    alure::UniquePtr<EnvEffects> al_efx;
    alure::UniquePtr<VoxManager> vox;

    char al_device_name[1024] = "";
    int al_device_majorversion = 0;
    int al_device_minorversion = 0;

    std::shared_ptr<AudioCache> cache;
    std::shared_ptr<SoundLoader> loader;

    //Print buffer
    std::string dprint_buffer;

    bool ChannelCheckIsPlaying(const aud_channel_t& channel);
    void S_FreeCache(sfx_t* sfx);
    void S_FlushCaches(void);
    void S_CheckWavEnd(aud_channel_t* ch, aud_sfxcache_t* sc);
    void SND_Spatialize(aud_channel_t* ch, qboolean init);
    void S_FreeChannel(aud_channel_t* ch);
    int S_AlterChannel(int entnum, int entchannel, sfx_t* sfx, float fvol, float pitch, int flags);
    bool SND_IsPlaying(sfx_t* sfx);
    aud_channel_t* SND_PickDynamicChannel(int entnum, int entchannel, sfx_t* sfx);
    aud_channel_t* SND_PickStaticChannel(int entnum, int entchannel, sfx_t* sfx);
    void S_StartSound(int entnum, int entchannel, sfx_t* sfx, float* origin, float fvol, float attenuation, int flags, int pitch, bool is_static);
    bool OpenAL_Init();
    

    void OpenAL_Shutdown();

  public:
    AudioEngine(std::shared_ptr<AudioCache> cache, std::shared_ptr<SoundLoader> loader);

    void AL_Version();
    void AL_ResetEFX();
    void AL_Devices(bool basic);

    void S_Startup();
    void S_Init();
    void S_Shutdown();
    void S_ShutdownAL();

    void S_StartDynamicSound(int entnum, int entchannel, sfx_t* sfx, float* origin, float fvol, float attenuation, int flags, int pitch);
    void S_StartStaticSound(int entnum, int entchannel, sfx_t* sfx, float* origin, float fvol, float attenuation, int flags, int pitch);
    void S_StopSound(int entnum, int entchannel);
    void S_StopAllSounds(qboolean clear);
    void S_Update(float* origin, float* forward, float* right, float* up);

    sfx_t* S_FindName(char* name, int* pfInCache);
  };
}