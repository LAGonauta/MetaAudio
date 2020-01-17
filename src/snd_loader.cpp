#include <iostream>
#include <fstream>

#include "FileSystem.h"
#include "alure2.h"
#include "snd_loader.hpp"

// Based on Alure's Stream class
class GoldSrcFileBuf final : public std::streambuf {
  alure::Array<char_type, 2048> mBuffer;
  FileHandle_t mFile{ nullptr };

  int_type underflow() override
  {
    if (mFile && gptr() == egptr())
    {
      auto got = g_pFileSystem->Read(mBuffer.data(), mBuffer.size(), mFile);
      if (got)
      {
        setg(mBuffer.data(), mBuffer.data(), mBuffer.data() + got);
      }
    }

    if (gptr() == egptr())
    {
      return traits_type::eof();
    }
    return traits_type::to_int_type(*gptr());
  }

  pos_type seekoff(off_type offset, std::ios_base::seekdir whence, std::ios_base::openmode mode) override
  {
    if (!mFile || (mode&std::ios_base::out) || !(mode&std::ios_base::in))
    {
      return traits_type::eof();
    }

    if (offset < 0)
    {
      return traits_type::eof();
    }

    auto seekType = FILESYSTEM_SEEK_HEAD;
    switch (whence)
    {
    case std::ios_base::beg:
      break;

    case std::ios_base::cur:
      seekType = FILESYSTEM_SEEK_CURRENT;
      if ((offset >= 0 && offset < off_type(egptr() - gptr())) ||
        (offset < 0 && -offset <= off_type(gptr() - eback())))
      {
        auto initialPos = g_pFileSystem->Tell(mFile);
        g_pFileSystem->Seek(mFile, static_cast<int>(offset), seekType);
        auto newPos = g_pFileSystem->Tell(mFile);
        if (newPos - initialPos != offset)
        {
          return traits_type::eof();
        }
        setg(eback(), gptr() + offset, egptr());
        return newPos - off_type(egptr() - gptr());
      }
      offset -= off_type(egptr() - gptr());
      break;

    case std::ios_base::end:
      offset += g_pFileSystem->Size(mFile);
      break;

    default:
      return traits_type::eof();
    }

    g_pFileSystem->Seek(mFile, static_cast<int>(offset), seekType);
    auto curPosition = g_pFileSystem->Tell(mFile);

    setg(nullptr, nullptr, nullptr);
    return curPosition;
  }

  pos_type seekpos(pos_type pos, std::ios_base::openmode mode) override
  {
    if (!mFile || (mode&std::ios_base::out) || !(mode&std::ios_base::in))
    {
      return traits_type::eof();
    }

    g_pFileSystem->Seek(mFile, static_cast<int>(pos), FILESYSTEM_SEEK_HEAD);
    if (g_pFileSystem->EndOfFile(mFile))
    {
      return traits_type::eof();
    }
    auto curPosition = g_pFileSystem->Tell(mFile);

    setg(nullptr, nullptr, nullptr);
    return curPosition;
  }

public:
  bool open(const char *filename) noexcept
  {
    mFile = g_pFileSystem->Open(filename, "rb");
    if (!mFile)
    {
      return false;
    }
    return true;
  }

  GoldSrcFileBuf() = default;
  ~GoldSrcFileBuf() override
  {
    g_pFileSystem->Close(mFile);
    mFile = nullptr;
  }
};

class GoldSrcFileStream final : public std::istream {
  GoldSrcFileBuf mStreamBuf;

public:
  GoldSrcFileStream(const char *filename) : std::istream(nullptr)
  {
    init(&mStreamBuf);

    if (!mStreamBuf.open(filename))
    {
      clear(failbit);
    }
  }
};

alure::UniquePtr<std::istream> GoldSrcFileFactory::openFile(const alure::String &name) noexcept
{
  alure::String namebuffer = "sound";

  if (name[0] != '/')
  {
    namebuffer.append("/");
  }

  namebuffer.append(name);

  auto fileExists = g_pFileSystem->FileExists(namebuffer.c_str());
  if (!fileExists)
  {
    namebuffer.clear();
    if (name[0] != '/')
    {
      namebuffer.append("/");
    }
    namebuffer.append(name);

    fileExists = g_pFileSystem->FileExists(namebuffer.c_str());
  }

  alure::UniquePtr<std::istream> file;
  if (fileExists)
  {
    char final_file_path[260]; // MAX_PATH
    g_pFileSystem->GetLocalPath(namebuffer.c_str(), final_file_path, sizeof(final_file_path));
    file = alure::MakeUnique<std::ifstream>(final_file_path, std::ios::binary);
    if (file->fail())
    {
      file = alure::MakeUnique<GoldSrcFileStream>(namebuffer.c_str());
      if (file->fail())
      {
        file = nullptr;
      }
      else
      {
        *file >> std::noskipws;
      }
    }
  }

  return std::move(file);
}
