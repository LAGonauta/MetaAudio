#include <metahook.h>

#include "snd_local.h"
#include "zone.h"

std::unordered_map<alure::String, aud_sfxcache_t> cache;

void Cache_Free(alure::String name)
{
  cache.erase(name);
}

void *Cache_Alloc(cache_user_t *c, alure::String name)
{
  if (c->data)
    Sys_ErrorEx("Cache_Alloc: already allocated");

  auto result = cache.emplace(std::make_pair(name, aud_sfxcache_t()));
  c->data = reinterpret_cast<void *>(&result.first->second);

  return c->data;
}