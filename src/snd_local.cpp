#include "snd_local.h"
#include "Vox/VoxManager.hpp"

aud_channel_t::~aud_channel_t()
{
  if (this->vox)
  {
    if (this->isentence >= 0)
    {
      auto& sentence = this->vox->rgrgvoxword[this->isentence];
      for (auto& word : sentence) word.sfx = nullptr;
    }

    this->vox->CloseMouth(this);
  }
}

aud_channel_t::aud_channel_t(aud_channel_t&& other) noexcept
{
  std::swap(sfx, other.sfx);
  std::swap(volume, other.volume);
  std::swap(pitch, other.pitch);
  std::swap(attenuation, other.attenuation);
  std::swap(entnum, other.entnum);
  std::swap(entchannel, other.entchannel);
  std::swap(origin, other.origin);
  std::swap(start, other.start);
  std::swap(end, other.end);
  std::swap(isentence, other.isentence);
  std::swap(iword, other.iword);
  std::swap(voicecache, other.voicecache);
  std::swap(sound_source, other.sound_source);
  std::swap(vox, other.vox);
  std::swap(firstpass, other.firstpass);
  std::swap(LowGain, other.LowGain);
  std::swap(MidGain, other.MidGain);
  std::swap(HighGain, other.HighGain);
}

aud_channel_t& aud_channel_t::operator=(aud_channel_t&& other) noexcept
{
  std::swap(sfx, other.sfx);
  std::swap(volume, other.volume);
  std::swap(pitch, other.pitch);
  std::swap(attenuation, other.attenuation);
  std::swap(entnum, other.entnum);
  std::swap(entchannel, other.entchannel);
  std::swap(origin, other.origin);
  std::swap(start, other.start);
  std::swap(end, other.end);
  std::swap(isentence, other.isentence);
  std::swap(iword, other.iword);
  std::swap(voicecache, other.voicecache);
  std::swap(sound_source, other.sound_source);
  std::swap(vox, other.vox);
  std::swap(firstpass, other.firstpass);
  std::swap(LowGain, other.LowGain);
  std::swap(MidGain, other.MidGain);
  std::swap(HighGain, other.HighGain);
  return *this;
}