#include <metahook.h>

#include "snd_local.h"
#include "Loaders/SoundLoader.hpp"

std::shared_ptr<MetaAudio::AudioCache> cache = std::make_shared<MetaAudio::AudioCache>();
MetaAudio::SoundLoader loader(cache);

aud_sfxcache_t *S_LoadSound(sfx_t *s, aud_channel_t *ch)
{
  return loader.S_LoadSound(s, ch);
}