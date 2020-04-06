#pragma once
#include "alure2.h"

namespace MetaAudio
{
  class GoldSrcFileFactory final : public alure::FileIOFactory
  {
  public:
    alure::UniquePtr<std::istream> openFile(const alure::String& name) noexcept override;
  };
}