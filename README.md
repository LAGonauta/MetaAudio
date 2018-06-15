# metahook
GoldSrc engine plugin extensions

# MetaAudio
This is a fork of MetaAudio, a GoldSrc plugin that adds OpenAL support to its sound system. This fork fixes some bugs and uses Alure instead of OpenAL directly for easier source management.

Thus we now have HRTF and surround sound back to our beloved GoldSrc games, and, as this plugin hooks directly into the engine, most mods should work with the new system.

Tested mods:
- Natural-selection

# Known bugs
- Mouth movements can be buggy.
- Some sounds are too quiet.

# TODO
- Possibly integrate Steam Audio (for occlusion and reflections).
- Fix bugs (obviously)
- Add support for voice communication.

# Compiling
Just load the MetaHook solution, set your post-build event to your desired folder and compile normally with Visual Studio.
Tested with MSVC 2017.

# Credits
nagist, for MetaHook (https://github.com/nagist/metahook)

hzqst, for the original MetaAudio (https://github.com/hzqst/MetaRenderer)
