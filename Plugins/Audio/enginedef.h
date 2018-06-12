#pragma once

#define	MAX_QPATH		64			// max length of a quake game pathname

#define MAX_SFX  1024

typedef struct sfx_s
{
	char 	name[MAX_QPATH];
	cache_user_t	cache;
	int		servercount;
}sfx_t;

typedef struct sfxcache_s
{
	int 	length;
	int 	loopstart;
	int 	speed;
	int 	width;
	int 	stereo;
	byte	data[1];		// variable sized
} sfxcache_t;

typedef struct channel_s
{
	sfx_t	*sfx;			// sfx number
	int		leftvol;		// 0-255 volume
	int		rightvol;		// 0-255 volume
	int		end;			// end time in global paintsamples
	int 	pos;			// sample position in sfx
	int		looping;		// where to loop, -1 = no looping
	int		entnum;			// to allow overriding a specific sound
	int		entchannel;		//
	vec3_t	origin;			// origin of sound effect
	vec_t	dist_mult;		// distance multiplier (attenuation/clipK)
	int		master_vol;		// 0-255 master volume
	int		isentence;
	int		iword;
	int		pitch;			// real-time pitch after any modulation or shift by dynamic data
}channel_t;

typedef struct voxword_s
{
	int		volume;						// increase percent, ie: 125 = 125% increase
	int		pitch;						// pitch shift up percent
	int		start;						// offset start of wave percent
	int		end;						// offset end of wave percent
	int		cbtrim;						// end of wave after being trimmed to 'end'
	int		fKeepCached;				// 1 if this word was already in cache before sentence referenced it
	int		samplefrac;					// if pitch shifting, this is position into wav * 256
	int		timecompress;				// % of wave to skip during playback (causes no pitch shift)
	sfx_t	*sfx;						// name and cache pointer
} voxword_t;

typedef struct wavinfo_s
{
	int		bps;
	int		align;
	int		rate;
	int		width;
	int		channels;
	int		loopstart;
	int		samples;
	int		dataofs;		// chunk starts this many bytes from file start
} wavinfo_t;

typedef struct sx_preset_s 
{
	float room_lp;					// for water fx, lowpass for entire room
	float room_mod;					// stereo amplitude modulation for room

	float room_size;				// reverb: initial reflection size
	float room_refl;				// reverb: decay time
	float room_rvblp;				// reverb: low pass filtering level

	float room_delay;				// mono delay: delay time
	float room_feedback;			// mono delay: decay time
	float room_dlylp;				// mono delay: low pass filtering level

	float room_left;				// left channel delay time
} sx_preset_t;

#define MAX_SOUNDS		512	

// a sound with no channel is a local only sound
#define SND_VOLUME		(1<<0)		// a byte
#define SND_ATTENUATION	(1<<1)		// a byte
#define SND_LARGE_INDEX	(1<<2)		// a long
#define SND_PITCH		(1<<3)
#define SND_SENTENCE	(1<<4)
#define SND_STOP		(1<<5)
#define SND_CHANGE_VOL	(1<<6)
#define SND_CHANGE_PITCH	(1<<7)
#define SND_SPAWNING	(1<<8)

#define	NUM_AMBIENTS			4		// automatic ambient sounds
#define	MAX_DYNAMIC_CHANNELS	8
#define MAX_CHANNELS			128

#define MAX_WAVSTREAM_SIZE	(32*1024)

#define CVOXWORDMAX		32
#define CVOXSENTENCEMAX	16

#define CVOXZEROSCANMAX	255

#define CVOXFILESENTENCEMAX		1536

#define CAVGSAMPLES 10

#define CSXROOM 29

extern voxword_t rgrgvoxword[CVOXSENTENCEMAX][CVOXWORDMAX];