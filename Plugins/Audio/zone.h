#pragma once

#if !defined(CACHE_USER)
#define CACHE_USER
typedef struct cache_user_s
{
  void *data;
}
cache_user_t;
#endif

void *Cache_Alloc(cache_user_t *c, alure::String name);
void Cache_Free(alure::String name);
