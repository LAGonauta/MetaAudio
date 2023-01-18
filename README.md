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
  - 0: Disabled.
  - 1: MetaAudio will also use the system clock to estimate when the sound effect ended. (preferred)
  - 2: MetaAudio will force all sound effects to be played as a stream.
- al_occluder (when SteamAudio is available. 0 is GoldSrc-based occluder, 1 is SteamAudio-based occluder)

# Console commands
- al_version (shows current MetaAudio version, along with the current OpenAL device and the OpenAL version)
- al_show_basic_devices (shows all basic OpenAL, such as just _OpenAL Soft_)
- al_show_full_devices (shows the full name of all OpenAL devices, such as _Generic Software on Realtek Speakers_)
- al_reset_efx (resets and re-creates all EFX sends, useful for buggy OpenAL drivers that were unable to enable a effect send for some unknown reason)

# Executable arguments
- al_maxsends (sets the max number of simultaneous EFX effects, MetaAudio uses a maximum of 2 but this can limit it to less)
- al_device (sets the OpenAL device to use, can be _OpenAL Soft_ or _OpenAL Soft on Realtek_, for example)

# Manual Installation

Copy the "metahook" folder to your mod dir.

Copy the other files to Steam's Half-Life dir.

Example folder structure, where <game> can be "valve", "cstrike", "ns", "gearbox" or any other mod:
```
|%STEAM%\steamapps\common\Half-Life\
|----> metahook.exe
|----> sndfile.dll
|----> OpenAL32.dll (remove to use X-Fi hardware acceleration)
|----> <game>\
  |----> metahook\
    |----> plugins\
      |----> MetaAudio.dll
    |----> configs\
      |----> plugins.lst
```
One should load the game through "MetaHook.exe". It is recommended to create a shortcut with at least the following launch options:
"-steam -insecure -game <game>". There are two sample shorcut files included, "OpenAL Half-Life" and "OpenAL Natural-Selection".

There should be an "MetaAudio.dll" entry in "plugins.lst".

# One Click Installation

1. You have to compile the MetaAudio by yourself before installation. see Compiling

2. Run `install-to-(WhateverGameYouWant).bat`

3. Launch game from shortcut `MetaHook for (WhateverGameYouWant)`

* You should have your Steam running otherwise the [SteamAppsLocation](SteamAppsLocation/README.md) will probably not going to find GameInstallDir.

# Known bugs

- Some sounds are too quiet
- Some sounds does not use the correct source origin

# Maybe in the future

- Integrate SteamAudio for reflections/reverb in addition to OpenAL's native effects

# Build Requirements

1. [Visual Studio 2017 / 2019 / 2022, with vc141 / vc142 / vc143 toolset](https://visualstudio.microsoft.com/)

2. [CMake](https://cmake.org/download/)

3. [Git for Windows](https://gitforwindows.org/)

# Compiling

1. Run `build-initdeps.bat`, wait until all required submodules / dependencies are pulled. (this may takes couple of minutes, depending on your network connection and download speed)

2. Run `build-MetaAudio.bat`, wail until all binary files generated.

(legacy) ~~Just load the MetaHook solution, set your post-build event to your desired folder and compile normally with Visual Studio.
Tested with MSVC 2019. Remember to install C++ CRT.~~

The include Alure2 has a slight different API than upstream, therefore must be compiled together.

# Debugging

1. Run `build-initdeps.bat`, wait until all required submodules / dependencies are pulled. (this may takes couple of minutes, depending on your network connection and download speed)

2. Run `debug-(WhateverGameYouWant).bat`, depends on which you are going to debug with

3. Open MetaHook.sln with Visual Studio IDE, set specified project as launch project, compile the project, then press F5 to start debugging.

* Other games follow the same instruction.

* You should restart Visual Studio IDE to apply changes to debugging profile, if Visual Studio IDE was running.

# Credits

nagist, for MetaHook (https://github.com/nagist/metahook)

hzqst, for the original MetaAudio (https://github.com/hzqst/MetaRenderer)

# Support

Support group available through an XMPP MUC:

xmpp:metaaudio@conference.xmpp.zone?join