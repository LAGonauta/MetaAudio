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

    void SetPitch(float pitch)
    {
      m_source->setPitch(pitch);
    }

    void SetGain(float gain)
    {
      m_source->setGain(gain);
    }

    virtual void SetLooping(bool value)
    {
      m_source->setLooping(value);
    }

    void Stop()
    {
      m_source->stop();
    }

    void SetDirectFilter(const alure::FilterParams& filter)
    {
      m_source->setDirectFilter(filter);
    }

    void SetDopplerFactor(float factor)
    {
      m_source->setDopplerFactor(factor);
    }

    void SetAuxiliarySendFilter(alure::AuxiliaryEffectSlot auxslot, ALuint send, const alure::FilterParams& filter)
    {
      m_source->setAuxiliarySendFilter(auxslot, send, filter);
    }

    void SetOffset(uint64_t offset)
    {
      m_source->setOffset(offset);
    }

    void SetRolloffFactors(float factor, float roomfactor)
    {
      m_source->setRolloffFactors(factor, roomfactor);
    }

    void SetDistanceRange(float refdist, float maxdist)
    {
      m_source->setDistanceRange(refdist, maxdist);
    }

    void SetAirAbsorptionFactor(float factor)
    {
      m_source->setAirAbsorptionFactor(factor);
    }

    uint64_t GetSampleOffset()
    {
      return m_source->getSampleOffset();
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