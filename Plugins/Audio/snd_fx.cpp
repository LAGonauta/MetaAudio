#include <metahook.h>
#include "exportfuncs.h"
#include "FileSystem.h"
#include "util.h"
#include "snd_local.h"
#include <math.h>
#include "alure/AL/efx-presets.h"

extern cvar_t *sxroom_off;
extern cvar_t *sxroomwater_type;
extern cvar_t *sxroom_type;

alure::Effect alReverbEffects[CSXROOM];
alure::AuxiliaryEffectSlot alAuxEffectSlots;

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
    EFX_REVERB_PRESET_OUTDOORS_BACKYARD,          // 20
    EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON,        // 21
    EFX_REVERB_PRESET_OUTDOORS_CREEK,             // 22
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
    if (roomtype > 0 && roomtype < CSXROOM && sxroom_off && !sxroom_off->value)
    {
        if (underwater)
        {
            alAuxEffectSlots.applyEffect(alReverbEffects[14]);
            ch->source.setAuxiliarySend(alAuxEffectSlots, 0);
            ch->source.setDirectFilter(alure::FilterParams{ 1.0f, 0.25f, AL_HIGHPASS_DEFAULT_GAIN });
        }
        else
        {
            alAuxEffectSlots.applyEffect(alReverbEffects[roomtype]);
            ch->source.setAuxiliarySend(alAuxEffectSlots, 0);
            ch->source.setDirectFilter(alure::FilterParams{ 1.0f, AL_LOWPASS_DEFAULT_GAIN, AL_HIGHPASS_DEFAULT_GAIN });
        }
    }
    else
    {
        alAuxEffectSlots.applyEffect(alReverbEffects[0]);
        ch->source.setAuxiliarySend(alAuxEffectSlots, 0);

        if (underwater)
            ch->source.setDirectFilter(alure::FilterParams{ 1.0f, 0.25, AL_HIGHPASS_DEFAULT_GAIN });
        else
            ch->source.setDirectFilter(alure::FilterParams{ 1.0f, AL_LOWPASS_DEFAULT_GAIN, AL_HIGHPASS_DEFAULT_GAIN });
    }
}

void SX_Init(void)
{
    alure::Context al_context = alure::Context::GetCurrent();

    // Disable reverb when room_type = 0:
    presets_room[0].flGain = 0;

    // Init each effect
    for (int i = 0; i < CSXROOM; ++i)
    {
        alReverbEffects[i] = al_context.createEffect();
        alReverbEffects[i].setReverbProperties(presets_room[i]);
    }

    alAuxEffectSlots = al_context.createAuxiliaryEffectSlot();
}

void SX_Shutdown(void)
{
    for (size_t i = 0; i < CSXROOM; ++i)
    {
        alReverbEffects[i].destroy();
    }
    alAuxEffectSlots.destroy();
}