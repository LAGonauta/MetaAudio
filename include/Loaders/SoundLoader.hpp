#pragma once
#include <metahook.h>
#include <optional>

#include "alure2.h"
#include "LoadingBufferHandler.hpp"
#include "../Utilities/AudioCache.hpp"
#include "SteamAudioMapMeshLoader.hpp"

namespace MetaAudio
{
  class SoundLoader final
  {
  private:
    alure::SharedPtr<AudioCache> m_cache = nullptr;
    alure::SharedPtr<LoadingBufferHandler> m_loading_handler = nullptr;

    // Check if file exists. Order: original, .wav, .flac, .ogg, .mp3
    std::optional<alure::String> S_GetFilePath(const alure::String& sfx_name, bool is_stream);
    aud_sfxcache_t* S_LoadStreamSound(sfx_t* s, aud_channel_t* ch);
  public:
    SoundLoader(const alure::SharedPtr<AudioCache>& cache);
    aud_sfxcache_t* S_LoadSound(sfx_t* s, aud_channel_t* ch);
  };
}