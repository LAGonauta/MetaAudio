# MetaHook
GoldSrc engine plugin extensions

# Warning
While the plugin is not a cheat, VAC may flag MetaHook and ban the user.
Do not use with Steam in online mode, we are not responsible if your account is banned.

# MetaAudio
This is a fork of MetaAudio, a GoldSrc plugin that adds OpenAL support to its sound system. This fork fixes some bugs and uses Alure instead of OpenAL directly for easier source management.

Thus we now have HRTF and surround sound back to our beloved GoldSrc games, and, as this plugin hooks directly into the engine, most mods should work with the new system.

Working games and mods:
- Half-Life
- Half-Life: Opposing Force
- Half-Life: Blue Shift
- Half-Life: C.A.G.E.D.
- Natural-Selection
- Counter-Strike
- Counter-Strike: Condition Zero
... and possibly more!

# What works
- Sound positioning
- Distance attenuation
- EFX effects
- Voice communication

# What is better than current GoldSrc
- FLAC and OGG support (if Alure2 was compiled with support)
- Occlusion support like A3D
- Fade between environmental effects
- Hardware acceleration (with X-RAM support)
- Significantly lower audio latency (especially if used with hardware acceleration)
- "Unlimited" heap size for audio

# Console variables
- al_doppler (sets the doppler effect intensity, 0.3 recommended)
- al_occlusion (enables or disables occlusion)
- al_occlusion_fade (enables or disables the smoothening of volumes changes of occluded sources)
- al_xfi_workaround (can be 0, 1 or 2. Fixes missing sounds when using a X-Fi with buggy OpenAL drivers.)
-- 0: Disabled.
-- 1: MetaAudio will also use the system clock to estimate when the sound effect ended. (preferred)
-- 2: MetaAudio will force all sound effects to be played as a stream.

# Console commands
- al_version (shows current MetaAudio version, along with the current OpenAL device and the OpenAL version)
- al_show_basic_devices (shows all basic OpenAL, such as just _OpenAL Soft_)
- al_show_full_devices (shows the full name of all OpenAL devices, such as _Generic Software on Realtek Speakers_)
- al_reset_efx (resets and re-creates all EFX sends, useful for buggy OpenAL drivers that were unable to enable a effect send for some unknown reason)

# Executable arguments
- al_maxsends (sets the max number of simultaneous EFX effects, MetaAudio uses a maximum of 2 but this can limit it to less)
- al_device (sets the OpenAL device to use, can be _OpenAL Soft_ or _OpenAL Soft on Realtek_, for example)

# Installing
Copy the "metahook" folder to your mod dir.
Copy the other files to Steam's Half-Life dir.

Example folder structure, where <game> can be "valve", "cstrike", "ns", "gearbox" or any other mod:
|%STEAM%\steamapps\common\Half-Life\
|----> Metahook.exe
|----> alure2.dll
|----> libsndfile-1.dll
|----> OpenAL32.dll (remove to use X-Fi hardware acceleration)
|----> <game>\
  |----> metahook\
    |----> plugins\
      |----> audio.dll
    |----> configs\
      |----> plugins.lst

One should load the game through "MetaHook.exe". It is recommended to create a shortcut with at least the following launch options:
"-steam -game <game> -insecure". There are two sample shorcut files included, "OpenAL Half-Life" and "OpenAL Natural-Selection".

There should be an "audio.dll" entry in "plugins.lst".

# Known bugs
- Some sounds are too quiet
- Some sounds does not use the correct source origin

# Maybe in the future
- Integrate Steam Audio for reflections/reverb instead of OpenAL's native effects

# Compiling
Just load the MetaHook solution, set your post-build event to your desired folder and compile normally with Visual Studio.
Tested with MSVC 2017. Remember to install C++ CRT.

You will also need to compile a custom Alure2 with slightly different API to use the audio plugin:

https://github.com/LAGonauta/alure/tree/hasLoopPoints

# Credits
nagist, for MetaHook (https://github.com/nagist/metahook)

hzqst, for the original MetaAudio (https://github.com/hzqst/MetaRenderer)

# Support
Support group available on FreeNode through IRC, come join: #metaaudio
