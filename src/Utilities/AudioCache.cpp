#include "Utilities/AudioCache.hpp"

namespace MetaAudio
{
  aud_sfxcache_t* AudioCache::Cache_Alloc(cache_user_t* c, const alure::String& name)
  {
    if (c->data)
    {
      Sys_ErrorEx("Cache_Alloc: already allocated");
    }

    auto& result = cache.emplace(name, aud_sfxcache_t());
    c->data = static_cast<void*>(&result.first->second);

    return &result.first->second;
  }

  void AudioCache::Cache_Free(const alure::String& name)
  {
    cache.erase(name);
  }
}