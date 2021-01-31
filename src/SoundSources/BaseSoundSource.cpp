#include "SoundSources/BaseSoundSource.hpp"

namespace MetaAudio
{
  BaseSoundSource::~BaseSoundSource()
  {
    if (m_source)
    {
      m_source->stop();
    }
  }

  bool BaseSoundSource::IsPlaying()
  {
    return m_source->isPlaying();
  }

  uint64_t BaseSoundSource::GetSampleOffset()
  {
    return m_source->getSampleOffset();
  }

  void BaseSoundSource::SetPosition(alure::Vector3 position)
  {
    m_source->setPosition(position);
  }

  void BaseSoundSource::SetRelative(bool value)
  {
    m_source->setRelative(value);
  }

  void BaseSoundSource::SetVelocity(alure::Vector3 velocity)
  {
    m_source->setVelocity(velocity);
  }

  void BaseSoundSource::SetRadius(float radius)
  {
    m_source->setRadius(radius);
  }

  void BaseSoundSource::SetPitch(float pitch)
  {
    m_source->setPitch(pitch);
  }

  void BaseSoundSource::SetGain(float gain)
  {
    m_source->setGain(gain);
  }

  void BaseSoundSource::SetLooping(bool value)
  {
    m_source->setLooping(value);
  }

  bool BaseSoundSource::IsLooping()
  {
    return m_source->getLooping();
  }

  void BaseSoundSource::Stop()
  {
    m_source->stop();
  }

  void BaseSoundSource::SetDirectFilter(const alure::FilterParams& filter)
  {
    m_source->setDirectFilter(filter);
  }

  void BaseSoundSource::SetDopplerFactor(float factor)
  {
    m_source->setDopplerFactor(factor);
  }

  void BaseSoundSource::SetAuxiliarySendFilter(alure::AuxiliaryEffectSlot auxslot, ALuint send, const alure::FilterParams& filter)
  {
    m_source->setAuxiliarySendFilter(auxslot, send, filter);
  }

  void BaseSoundSource::SetOffset(uint64_t offset)
  {
    m_source->setOffset(offset);
  }

  void BaseSoundSource::SetRolloffFactors(float factor, float roomfactor)
  {
    m_source->setRolloffFactors(factor, roomfactor);
  }

  void BaseSoundSource::SetDistanceRange(float refdist, float maxdist)
  {
    m_source->setDistanceRange(refdist, maxdist);
  }

  void BaseSoundSource::SetAirAbsorptionFactor(float factor)
  {
    m_source->setAirAbsorptionFactor(factor);
  }
}