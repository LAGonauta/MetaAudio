#include <metahook.h>
#include "exportfuncs.h"
#include "FileSystem.h"
#include "util.h"
#include "snd_local.h"
#include "zone.h"
#include <math.h>

extern cvar_t *sxroom_off;
extern cvar_t *sxroomwater_type;
extern cvar_t *sxroom_type;

ALuint alUnderWaterFilter;
ALuint alReverbEffects[CSXROOM];
ALuint alAuxEffectSlots;

typedef struct
{
	float size;
	float feedback;
	int lowpass;
	float decay;
	float delay;
	float mod;;
}al_room_t;

al_room_t presets_room[CSXROOM] = {
	{0, 0, 1, 0, 0, 0},
	//SXROOM_GENERIC
	{0, 0.1, 1, 0, 0.065, 0},
	//SXROOM_METALIC_S
	{0, 0.75, 1, 0, 0.02, 0},
	{0, 0.78, 1, 0, 0.03, 0},
	{0, 0.77, 1, 0, 0.06, 0},
	//SXROOM_TUNNEL_S
	{0.05, 0.7, 1, 0.85, 0.018, 0},
	{0.05, 0.7, 1, 0.85, 0.020, 0},
	{0.05, 0.7, 1, 0.85, 0.025, 0},
	//SXROOM_CHAMBER_S
	{0.05, 0, 1, 0.84, 0, 0},
	{0.05, 0, 1, 0.90, 0, 0},
	{0.05, 0, 1, 0.95, 0, 0},
	//SXROOM_BRITE_S
	{0.05, 0, 0, 0.7, 0, 0},
	{0.05, 0, 0, 0.78, 0, 0},
	{0.05, 0, 0, 0.86, 0, 0},
	//SXROOM_WATER1
	{0, 0, 1, 0, 0, 0},
	{0, 0.85, 1, 0, 0.06, 0},
	{0, 0.6, 1, 0, 0.2, 0},
	//SXROOM_CONCRETE_S
	{0.05, 0.48, 1, 0.8, 0, 0},
	{0.06, 0.52, 1, 0.9, 0, 0},
	{0.07, 0.6, 1, 0.94, 0.3, 0},
	//SXROOM_OUTSIDE1
	{0, 0.42, 1, 0, 0.3, 0},
	{0, 0.48, 1, 0, 0.35, 0},
	{0, 0.6, 1, 0, 0.38, 0},
	//SXROOM_CAVERN_S
	{0.05, 0.28, 1, 0.9, 0.2, 0},
	{0.07, 0.4, 1, 0.9, 0.3, 0},
	{0.09, 0.5, 1, 0.9, 0.35, 0},
	//SXROOM_WEIRDO1
	{0.01, 0, 0, 0.9, 0, 1.0},
	{0.01, 0.999, 1, 0, 0.009, 0},
	{0.01, 0.8, 0, 0.999, 0.2, 0}
};	

void SX_ApplyEffect(aud_channel_t *ch, int roomtype, qboolean underwater)
{
	if(!qal_efxinit)
		return;

	if(roomtype > 0 && roomtype < CSXROOM && sxroom_off && !sxroom_off->value)
	{
		qalAuxiliaryEffectSloti(alAuxEffectSlots, AL_EFFECTSLOT_EFFECT, alReverbEffects[roomtype]);
		if(underwater)
		{
			qalSource3i(ch->source, AL_AUXILIARY_SEND_FILTER, alAuxEffectSlots, 0, alUnderWaterFilter);
			qalSourcei(ch->source, AL_DIRECT_FILTER, alUnderWaterFilter);
		}
		else
		{
			qalSource3i(ch->source, AL_AUXILIARY_SEND_FILTER, alAuxEffectSlots, 0, 0);
			qalSourcei(ch->source, AL_DIRECT_FILTER, 0);
		}
	}
	else
	{
		qalSource3i(ch->source, AL_AUXILIARY_SEND_FILTER, 0, 0, 0);
		if(underwater)
			qalSourcei(ch->source, AL_DIRECT_FILTER, alUnderWaterFilter);
		else
			qalSourcei(ch->source, AL_DIRECT_FILTER, 0);
	}
}

ALboolean SX_InitReverbEffect(al_room_t *RoomParam, ALuint uiEffect)
{
	ALboolean bReturn = AL_FALSE;

	qalGetError();

	qalEffecti(uiEffect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB );
	qalEffectf(uiEffect, AL_EAXREVERB_DENSITY, min( pow(RoomParam->size, 3.0f) * 0.0625, 1) );
	qalEffectf(uiEffect, AL_EAXREVERB_GAINLF, RoomParam->lowpass ? 0.25 : AL_EAXREVERB_DEFAULT_GAINLF);
	qalEffectf(uiEffect, AL_EAXREVERB_DECAY_TIME, RoomParam->decay);
	qalEffectf(uiEffect, AL_EAXREVERB_REFLECTIONS_DELAY, RoomParam->delay);
	qalEffectf(uiEffect, AL_EAXREVERB_REFLECTIONS_GAIN, RoomParam->feedback);
	qalEffectf(uiEffect, AL_EAXREVERB_MODULATION_DEPTH, RoomParam->mod);

	if (qalGetError() == AL_NO_ERROR)
	{
		bReturn = AL_TRUE;
	}

	return bReturn;
}

void SX_Init(void)
{
	if(!qal_efxinit)
		return;

	//Init underwater filter
	qalGenFilters(1, &alUnderWaterFilter);
	qalFilteri(alUnderWaterFilter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
	qalFilterf(alUnderWaterFilter, AL_LOWPASS_GAIN, AL_LOWPASS_DEFAULT_GAIN);
	qalFilterf(alUnderWaterFilter, AL_LOWPASS_GAINHF, 0.25f);

	//Init reverb effects
	qalGenEffects(CSXROOM-1, alReverbEffects);
	qalGenAuxiliaryEffectSlots(1, &alAuxEffectSlots);
	for(int i = 0; i < CSXROOM; ++i)
	{
		SX_InitReverbEffect(&presets_room[i], alReverbEffects[i]);
	}
}

void SX_Shutdown(void)
{
	if(!qal_efxinit)
		return;

	qalAuxiliaryEffectSloti(alAuxEffectSlots, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
	qalDeleteEffects(CSXROOM-1, alReverbEffects);
	qalDeleteAuxiliaryEffectSlots(1, &alAuxEffectSlots);

	qalDeleteFilters(1, &alUnderWaterFilter);
}