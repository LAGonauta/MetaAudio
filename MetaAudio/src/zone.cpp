#include <metahook.h>

#include "snd_local.h"
#include "zone.h"

std::unordered_map<alure::String, aud_sfxcache_t> cache;

void Cache_Free(alure::String name)
{
  cache.erase(name);
}

aud_sfxcache_t* Cache_Alloc(cache_user_t *c, alure::String name)
{
  if (c->data)
    Sys_ErrorEx("Cache_Alloc: already allocated");

  auto result = cache.emplace(name, aud_sfxcache_t());
  c->data = static_cast<void *>(&result.first->second);

  return &result.first->second;
}