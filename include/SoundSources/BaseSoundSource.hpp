#pragma once
#include "alure2.h"

namespace MetaAudio
{
  class BaseSoundSource
  {
  protected:
    alure::AutoObj<alure::Source> m_source;

  public:
    ~BaseSoundSource();

    virtual void Play() = 0;

    virtual bool IsPlaying();

    void SetPosition(alure::Vector3 position);

    void SetRelative(bool value);

    virtual void SetVelocity(alure::Vector3 velocity);

    virtual void SetRadius(float radius);

    void SetPitch(float pitch);

    void SetGain(float gain);

    void SetLooping(bool value);

    bool IsLooping();

    void Stop();

    void SetDirectFilter(const alure::FilterParams& filter);

    void SetDopplerFactor(float factor);

    void SetAuxiliarySendFilter(alure::AuxiliaryEffectSlot auxslot, ALuint send, const alure::FilterParams& filter);

    void SetOffset(uint64_t offset);

    void SetRolloffFactors(float factor, float roomfactor);

    void SetDistanceRange(float refdist, float maxdist);

    void SetAirAbsorptionFactor(float factor);

    uint64_t GetSampleOffset();
  };
}