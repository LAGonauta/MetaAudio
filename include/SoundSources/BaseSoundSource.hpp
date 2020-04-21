#pragma once
#include <chrono>

#include "alure2.h"

namespace MetaAudio
{
  class BaseSoundSource
  {
  private:


  protected:
    alure::AutoObj<alure::Source> m_source;

  public:
    virtual void Play() = 0;

    virtual bool IsPlaying()
    {
      return m_source->isPlaying();
    }

    alure::Source GetInternalSourceHandle()
    {
      return *m_source;
    }

    ~BaseSoundSource()
    {
      if (m_source)
      {
        m_source->stop();
      }
    }
  };
}