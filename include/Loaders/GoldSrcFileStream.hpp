#pragma once

#include "GoldSrcFileBuf.hpp"

namespace MetaAudio
{
  class GoldSrcFileStream final : public std::istream
  {
  private:
    GoldSrcFileBuf mStreamBuf;

  public:
    GoldSrcFileStream(const char* filename);
  };
}