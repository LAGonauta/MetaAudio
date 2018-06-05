#include <metahook.h>
#include <cvardef.h>
#include "plugins.h"
#include "exportfuncs.h"
#include "enginedef.h"
#include "snd_local.h"
#include "zone.h"
#include "plat.h"

byte	*data_p;
byte 	*iff_end;
byte 	*last_chunk;
byte 	*iff_data;
int 	iff_chunk_len;

extern cvar_t *loadas8bit;

short GetLittleShort(void)
{
	short val = 0;
	val = *data_p;
	val = val + (*(data_p+1)<<8);
	data_p += 2;
	return val;
}

int GetLittleLong(void)
{
	int val = 0;
	val = *data_p;
	val = val + (*(data_p+1)<<8);
	val = val + (*(data_p+2)<<16);
	val = val + (*(data_p+3)<<24);
	data_p += 4;
	return val;
}

void FindNextChunk(char *name)
{
	while (1)
	{
		data_p=last_chunk;

		if (data_p >= iff_end)
		{	// didn't find the chunk
			data_p = NULL;
			return;
		}

		data_p += 4;
		iff_chunk_len = GetLittleLong();
		if (iff_chunk_len < 0)
		{
			data_p = NULL;
			return;
		}
		//		if (iff_chunk_len > 1024*1024)
		//			Sys_Error ("FindNextChunk: %i length is past the 1 meg sanity limit", iff_chunk_len);
		data_p -= 8;
		last_chunk = data_p + 8 + ( (iff_chunk_len + 1) & ~1 );
		if (!strncmp((const char *)data_p, name, 4))
			return;
	}
}

void FindChunk(char *name)
{
	last_chunk = iff_data;
	FindNextChunk (name);
}

qboolean GetWavinfo(wavinfo_t *info, char *name, byte *wav, int wavlength)
{
	int     i;
	int     format;
	int		samples;

	memset(info, 0, sizeof(*info));

	if (!wav)
		return false;

	iff_data = wav;
	iff_end = wav + wavlength;

	// find "RIFF" chunk
	FindChunk("RIFF");
	if (!(data_p && !strncmp((const char *)(data_p+8), "WAVE", 4)))
	{
		gEngfuncs.Con_Printf("Missing RIFF/WAVE chunks\n");
		return false;
	}

	// get "fmt " chunk
	iff_data = data_p + 12;
	// DumpChunks ();

	FindChunk("fmt ");
	if (!data_p)
	{
		gEngfuncs.Con_Printf("Missing fmt chunk\n");
		return false;
	}
	data_p += 8;
	format = GetLittleShort();
	if (format != 1)
	{
		gEngfuncs.Con_Printf("Microsoft PCM format only\n");
		return false;
	}

	info->channels = GetLittleShort();
	info->rate = GetLittleLong();
	info->bps = GetLittleLong();
	info->align = GetLittleShort();
	//data_p += 4+2;
	info->width = GetLittleShort() / 8;

	// get cue chunk
	FindChunk("cue ");
	if (data_p)
	{
		data_p += 32;
		info->loopstart = GetLittleLong();
		//		Con_Printf("loopstart=%d\n", sfx->loopstart);

		// if the next chunk is a LIST chunk, look for a cue length marker
		FindNextChunk ("LIST");
		if (data_p)
		{
			if (!strncmp ((const char *)(data_p + 28), "mark", 4))
			{	// this is not a proper parse, but it works with cooledit...
				data_p += 24;
				i = GetLittleLong();	// samples in loop
				info->samples = info->loopstart + i;
			}
		}
	}
	else
		info->loopstart = -1;

	// find data chunk
	FindChunk("data");
	if (!data_p)
	{
		gEngfuncs.Con_Printf("Missing data chunk\n");
		return false;
	}

	data_p += 4;
	samples = GetLittleLong () / (info->width * info->channels);

	if (info->samples)
	{
		if (samples < info->samples)
			Sys_ErrorEx("Sound %s has a bad loop length", name);
	}
	else
		info->samples = samples;

	info->dataofs = data_p - wav;

	return true;
}

/*void ResampleSfx(aud_sfxcache_t *sc, byte *indata, byte *outdata)
{
	int i, srclength, outcount, fracstep, chancount;
	int	samplefrac, srcsample, srcnextsample;

	// this is usually 0.5 (128), 1 (256), or 2 (512)
	fracstep = 256;

	chancount = sc->channels - 1;
	srclength = sc->length * sc->channels;
	outcount = sc->length;

	int tochannels = 1;

// resample / decimate to the current source rate
	if (fracstep == 256)
	{
		if (sc->width == 2)
		{
			for (i = 0; i < srclength; i++)
				((int16 *)outdata)[i] = LittleShort (((int16 *)indata)[i]);
		} 
		else 
		{ // 8bit
			memcpy( outdata, indata, srclength );
		}
	}
	else
	{
		int j, a, b, sample;

// general case
		samplefrac = 0;
		srcsample = 0;
		srcnextsample = tochannels;
		outcount *= tochannels;

#define RESAMPLE_AND_ADVANCE	\
				sample = (((b - a) * (samplefrac & 255)) >> 8) + a; \
				if (j == chancount) \
				{ \
					samplefrac += fracstep; \
					srcsample = (samplefrac >> 8) << chancount; \
					srcnextsample = srcsample + sc->channels; \
				}

		if (sc->width == 2)
		{
			int16 *out = (int16 *)outdata, *in = (int16 *)indata;
			for (i = 0, j = 0; i < outcount; i++, j = i & chancount) {
				a = LittleShort (in[srcsample + j]);
				b = ((srcnextsample < srclength) ? LittleShort (in[srcnextsample + j]) : 0);
				RESAMPLE_AND_ADVANCE;
				*out++ = (int16)sample;
			}
		}
		else
		{
			byte *out = outdata, *in = indata;
			for (i = 0, j = 0; i < outcount; i++, j = i & chancount) {
				a = (int)in[srcsample + j];
				b = ((srcnextsample < srclength) ? (int)in[srcnextsample + j] : 128);
				RESAMPLE_AND_ADVANCE;
				*out++ = (byte)sample;
			}
		}
	}
}*/