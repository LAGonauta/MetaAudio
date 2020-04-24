#pragma once
#include "alure2.h"

namespace MetaAudio
{
  class BaseSoundSource
  {
  protected:
    alure::AutoObj<alure::Source> m_source;

  public:
    virtual void Play() = 0;

    virtual bool IsPlaying()
    {
      return m_source->isPlaying();
    }

    virtual void SetPosition(alure::Vector3 position)
    {
      m_source->setPosition(position);
    }

    virtual void SetRelative(bool value)
    {
      m_source->setRelative(value);
    }

    virtual void SetVelocity(alure::Vector3 velocity)
    {
      m_source->setVelocity(velocity);
    }

    virtual void SetRadius(float radius)
    {
      m_source->setRadius(radius);
    }

    virtual void SetLooping(bool value)
    {
      m_source->setLooping(value);
    }

    void Stop()
    {
      m_source->stop();
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