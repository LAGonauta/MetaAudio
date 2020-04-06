#include "Loaders/GoldSrcFileStream.hpp"

namespace MetaAudio
{
  GoldSrcFileStream::GoldSrcFileStream(const char* filename) : std::istream(nullptr)
  {
    init(&mStreamBuf);

    if (!mStreamBuf.open(filename))
    {
      clear(failbit);
    }
  }
}