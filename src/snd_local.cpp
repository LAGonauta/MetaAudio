#include "snd_local.h"
#include "Vox/VoxManager.hpp"

aud_channel_t::~aud_channel_t()
{
  if (this->vox)
  {
    this->vox->CloseMouth(this);
  }
}