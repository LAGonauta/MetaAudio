// Copyright 2018
// Licensed under GPLv3
// Refer to the LICENSE.md file included.

#pragma once

#include "snd_local.h"

class LocalAudioDecoder final : public alure::MessageHandler {
public:
  static const alure::Array<alure::String, 3> SupportedExtensions;
  bool GetWavinfo(wavinfo_t& info, alure::String full_path, alure::Vector<ALubyte>& data_output);
  void bufferLoading(alure::StringView name, alure::ChannelConfig channels, alure::SampleType type, ALuint samplerate, alure::ArrayView<ALbyte> data) noexcept override;

private:
  // To return the data to the application we copy the information here
  alure::StringView m_name;
  alure::ChannelConfig m_channels;
  alure::SampleType m_type;
  ALuint m_samplerate;
  alure::Vector<ALubyte> m_data;
};
