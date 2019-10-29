#ifndef IAUDIO_H
#define IAUDIO_H

#ifdef _WIN32
#pragma once
#endif

#ifndef AUDIO_EXPORTS
#include "aud_int.h"
#else
#include "aud_int_internal.h"
#endif

class IAudio : public IBaseInterface
{
public:
  virtual void GetInterface(aud_export_t *pAudExports, const char *version);
};

#define AUDIO_API_VERSION "AUDIO_API_VERSION001"

extern IAudio *g_pAudio;

#endif