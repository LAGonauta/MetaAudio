**DO NOT USE WITH STEAM ONLINE, OR IN VAC SERVERS. YOU RISK GETTING BANNED.**

**I AM NOT RESPONSIBLE FOR YOUR BAN, USE AT YOUR OWN RISK.**

# MetaHook
GoldSrc engine plugin extensions

# MetaAudio
This is a fork of MetaAudio, a GoldSrc plugin that adds OpenAL support to its sound system. This fork fixes some bugs and uses Alure instead of OpenAL directly for easier source management.

Thus we now have HRTF and surround sound back to our beloved GoldSrc games, and, as this plugin hooks directly into the engine, most mods should work with the new system.

Tested mods:
- Natural-selection

# What works
- Sound positioning
- Distance attenuation
- EFX effects
- Hardware acceleration (and X-RAM)

# What does not work
- Voice communication

# What is better than current GoldSrc
- Occlusion support like A3D
- Fade between environmental effects

# Known bugs
- Some sounds are too quiet
- Some sounds does not use the correct source origin

# TODO
- Possibly integrate Steam Audio (for reflections/reverb instead of OpenAL's native effects)
- Add support for voice communication

# Compiling
Just load the MetaHook solution, set your post-build event to your desired folder and compile normally with Visual Studio.
Tested with MSVC 2017. Remember to install C++ CRT.

You will probably also need to compile Alure2 to use the audio plugin:

https://github.com/kcat/alure/

# Credits
nagist, for MetaHook (https://github.com/nagist/metahook)

hzqst, for the original MetaAudio (https://github.com/hzqst/MetaRenderer)
