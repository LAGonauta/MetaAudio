#pragma once

#include "snd_local.h"

class VOX final
{
private:
  alure::Array<char *, CVOXWORDMAX> rgpparseword{};  // array of pointers to parsed words
  char voxperiod[8] = "_period";                     // vocal pause
  char voxcomma[7] = "_comma";                       // vocal pause
  voxword_t voxwordDefault;

  // Voice file lookup
  char *LookupString(char *pszin, int *psentencenum);
  char *GetDirectory(alure::String& szpath, char *psz);
  void ParseString(char *psz);
  int ParseWordParams(char *psz, voxword_t *pvoxword, int fFirst);
  int IFindEmptySentence(void);

  // Mouth movement
  void ForceInitMouth(int entnum);
  void ForceCloseMouth(int entnum);

public:
  alure::Array<alure::Array<voxword_t, CVOXWORDMAX>, CVOXSENTENCEMAX> rgrgvoxword;

  VOX();
  ~VOX();

  void TrimStartEndTimes(aud_channel_t *ch, aud_sfxcache_t *sc);
  void SetChanVolPitch(aud_channel_t *ch, float *fvol, float *fpitch);
  void ReadSentenceFile(void);
  aud_sfxcache_t *LoadSound(aud_channel_t *pchan, char *pszin);
  void MakeSingleWordSentence(aud_channel_t *ch, int pitch);
  void InitMouth(int entnum, int entchannel);
  void CloseMouth(aud_channel_t *ch);
  void MoveMouth(aud_channel_t *ch, aud_sfxcache_t *sc);
};

