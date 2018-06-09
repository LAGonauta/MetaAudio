#include <metahook.h>
#include "exportfuncs.h"
#include "FileSystem.h"
#include "util.h"
#include "snd_local.h"
#include "zone.h"
#include <math.h>
#include "OpenAL/efx-presets.h"

extern cvar_t *sxroom_off;
extern cvar_t *sxroomwater_type;
extern cvar_t *sxroom_type;

ALuint alUnderWaterFilter;
ALuint alReverbEffects[CSXROOM];
ALuint alAuxEffectSlots;

EFXEAXREVERBPROPERTIES presets_room[CSXROOM] = {
    EFX_REVERB_PRESET_GENERIC,                    //  0
	//SXROOM_GENERIC
    EFX_REVERB_PRESET_GENERIC,                    //  1
	//SXROOM_METALIC_S
    EFX_REVERB_PRESET_PIPE_SMALL,                 //  2
    EFX_REVERB_PRESET_PIPE_LONGTHIN,              //  3
    EFX_REVERB_PRESET_PIPE_RESONANT,              //  4
	//SXROOM_TUNNEL_S
    EFX_REVERB_PRESET_HALLWAY,                    //  4
    EFX_REVERB_PRESET_CASTLE_HALL,                //  6
    EFX_REVERB_PRESET_FACTORY_HALL,               //  7
	//SXROOM_CHAMBER_S
    EFX_REVERB_PRESET_SPACESTATION_SMALLROOM,     //  8
    EFX_REVERB_PRESET_SPACESTATION_MEDIUMROOM,    //  9
    EFX_REVERB_PRESET_SPACESTATION_LARGEROOM,     // 10
	//SXROOM_BRITE_S
    EFX_REVERB_PRESET_CASTLE_SMALLROOM,           // 11
    EFX_REVERB_PRESET_CASTLE_MEDIUMROOM,          // 12
    EFX_REVERB_PRESET_CASTLE_LARGEROOM,           // 13
	//SXROOM_WATER1
    EFX_REVERB_PRESET_UNDERWATER,                 // 14
    EFX_REVERB_PRESET_UNDERWATER,                 // 15
    EFX_REVERB_PRESET_UNDERWATER,                 // 16
	//SXROOM_CONCRETE_S
    EFX_REVERB_PRESET_PREFAB_WORKSHOP,            // 17
    EFX_REVERB_PRESET_PREFAB_SCHOOLROOM,          // 18
    EFX_REVERB_PRESET_PREFAB_PRACTISEROOM,        // 19
	//SXROOM_OUTSIDE1
    EFX_REVERB_PRESET_OUTDOORS_BACKYARD,                     // 20
    EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON,                      // 21
    EFX_REVERB_PRESET_OUTDOORS_CREEK,                       // 22
	//SXROOM_CAVERN_S
    EFX_REVERB_PRESET_CAVE,                       // 23
    EFX_REVERB_PRESET_CAVE,                       // 24
    EFX_REVERB_PRESET_CAVE,                       // 25
	//SXROOM_WEIRDO1
    EFX_REVERB_PRESET_MOOD_HEAVEN,                // 26
    EFX_REVERB_PRESET_MOOD_HELL,                  // 27
    EFX_REVERB_PRESET_MOOD_MEMORY                 // 28
};	

void SX_ApplyEffect(aud_channel_t *ch, int roomtype, qboolean underwater)
{
    if (!qal_efxinit)
        return;

    if (roomtype > 0 && roomtype < CSXROOM && sxroom_off && !sxroom_off->value)
    {
        if (underwater)
        {
            qalAuxiliaryEffectSloti(alAuxEffectSlots, AL_EFFECTSLOT_EFFECT, alReverbEffects[14]);
            qalSource3i(ch->source, AL_AUXILIARY_SEND_FILTER, alAuxEffectSlots, 0, alUnderWaterFilter);
            qalSourcei(ch->source, AL_DIRECT_FILTER, alUnderWaterFilter);
        }
        else
        {
            qalAuxiliaryEffectSloti(alAuxEffectSlots, AL_EFFECTSLOT_EFFECT, alReverbEffects[roomtype]);
            qalSource3i(ch->source, AL_AUXILIARY_SEND_FILTER, alAuxEffectSlots, 0, 0);
            qalSourcei(ch->source, AL_DIRECT_FILTER, 0);
        }
    }
    else
    {
        qalSource3i(ch->source, AL_AUXILIARY_SEND_FILTER, 0, 0, 0);
        if (underwater)
            qalSourcei(ch->source, AL_DIRECT_FILTER, alUnderWaterFilter);
        else
            qalSourcei(ch->source, AL_DIRECT_FILTER, 0);
    }
}

ALboolean SX_InitReverbEffect(EFXEAXREVERBPROPERTIES *RoomParam, ALuint uiEffect)
{
    ALboolean bReturn = AL_FALSE;

    qalGetError();

    qalEffecti(uiEffect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
    qalEffectf(uiEffect, AL_EAXREVERB_DENSITY, RoomParam->flDensity);
    qalEffectf(uiEffect, AL_EAXREVERB_DIFFUSION, RoomParam->flDiffusion);
    qalEffectf(uiEffect, AL_EAXREVERB_GAIN, RoomParam->flGain);
    qalEffectf(uiEffect, AL_EAXREVERB_GAINHF, RoomParam->flGainHF);
    qalEffectf(uiEffect, AL_EAXREVERB_GAINLF, RoomParam->flGainLF);
    qalEffectf(uiEffect, AL_EAXREVERB_DECAY_TIME, RoomParam->flDecayTime);
    qalEffectf(uiEffect, AL_EAXREVERB_DECAY_HFRATIO, RoomParam->flDecayHFRatio);
    qalEffectf(uiEffect, AL_EAXREVERB_DECAY_LFRATIO, RoomParam->flDecayLFRatio);
    qalEffectf(uiEffect, AL_EAXREVERB_REFLECTIONS_GAIN, RoomParam->flReflectionsGain);
    qalEffectf(uiEffect, AL_EAXREVERB_REFLECTIONS_DELAY, RoomParam->flReflectionsDelay);
    qalEffectfv(uiEffect, AL_EAXREVERB_REFLECTIONS_PAN, RoomParam->flReflectionsPan);
    qalEffectf(uiEffect, AL_EAXREVERB_LATE_REVERB_GAIN, RoomParam->flLateReverbGain);
    qalEffectf(uiEffect, AL_EAXREVERB_LATE_REVERB_DELAY, RoomParam->flLateReverbDelay);
    qalEffectfv(uiEffect, AL_EAXREVERB_LATE_REVERB_PAN, RoomParam->flLateReverbPan);
    qalEffectf(uiEffect, AL_EAXREVERB_ECHO_TIME, RoomParam->flEchoTime);
    qalEffectf(uiEffect, AL_EAXREVERB_ECHO_DEPTH, RoomParam->flEchoDepth);
    qalEffectf(uiEffect, AL_EAXREVERB_MODULATION_TIME, RoomParam->flModulationTime);
    qalEffectf(uiEffect, AL_EAXREVERB_MODULATION_DEPTH, RoomParam->flModulationDepth);
    qalEffectf(uiEffect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, RoomParam->flAirAbsorptionGainHF);
    qalEffectf(uiEffect, AL_EAXREVERB_HFREFERENCE, RoomParam->flHFReference);
    qalEffectf(uiEffect, AL_EAXREVERB_LFREFERENCE, RoomParam->flLFReference);
    qalEffectf(uiEffect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, RoomParam->flRoomRolloffFactor);
    qalEffecti(uiEffect, AL_EAXREVERB_DECAY_HFLIMIT, (RoomParam->iDecayHFLimit ? AL_TRUE : AL_FALSE));

    if (qalGetError() == AL_NO_ERROR)
    {
        bReturn = AL_TRUE;
    }

    return bReturn;
}

void SX_Init(void)
{
    if (!qal_efxinit)
        return;

    //Init underwater filter
    qalGenFilters(1, &alUnderWaterFilter);
    qalFilteri(alUnderWaterFilter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    qalFilterf(alUnderWaterFilter, AL_LOWPASS_GAIN, AL_LOWPASS_DEFAULT_GAIN);
    qalFilterf(alUnderWaterFilter, AL_LOWPASS_GAINHF, 0.25f);

    //Init reverb effects
    qalGenEffects(CSXROOM - 1, alReverbEffects);
    qalGenAuxiliaryEffectSlots(1, &alAuxEffectSlots);

    // Disable reverb when room_type = 0:
    presets_room[0].flGain = 0;
    for (int i = 0; i < CSXROOM; ++i)
    {
        SX_InitReverbEffect(&presets_room[i], alReverbEffects[i]);
    }
}

void SX_Shutdown(void)
{
    if (!qal_efxinit)
        return;

    qalAuxiliaryEffectSloti(alAuxEffectSlots, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
    qalDeleteEffects(CSXROOM - 1, alReverbEffects);
    qalDeleteAuxiliaryEffectSlots(1, &alAuxEffectSlots);

    qalDeleteFilters(1, &alUnderWaterFilter);
}