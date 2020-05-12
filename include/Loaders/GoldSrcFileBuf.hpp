#pragma once

#include "FileSystem.h"
#include "alure2.h"

namespace MetaAudio
{
  // Based on Alure's Stream class
  class GoldSrcFileBuf final : public std::streambuf
  {
  private:
    alure::Array<char_type, 2048> mBuffer;
    FileHandle_t mFile{ nullptr };

    int_type underflow() override;

    pos_type seekoff(off_type offset, std::ios_base::seekdir whence, std::ios_base::openmode mode) override;

    pos_type seekpos(pos_type pos, std::ios_base::openmode mode) override;

  public:
    bool open(const char* filename) noexcept;

    GoldSrcFileBuf() = default;
    ~GoldSrcFileBuf() override;
  };
}