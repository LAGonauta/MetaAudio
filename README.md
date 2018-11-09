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
