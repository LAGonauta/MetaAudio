#pragma once
#include <metahook.h>

#include "alure2.h"
#include "snd_local.h"

namespace MetaAudio
{
  class AudioCache final
  {
  private:
    std::unordered_map<alure::String, aud_sfxcache_t> cache;
  public:
    aud_sfxcache_t* Cache_Alloc(cache_user_t* c, const alure::String& name);
    void Cache_Free(const alure::String& name);
  };
}