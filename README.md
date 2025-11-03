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
- SvEngine (Sven Co-op has it's own sound engine integrated in the client dll, so theoretically MetaAudio won't affect anything unless you install some third-party mods into SvEngine)

... and possibly more!

Working engine:

- GoldSrc 3266
- GoldSrc 4554
- GoldSrc 6153
- GoldSrc 8684 (Pre-25th)
- GoldSrc 10240 (HL25th)
- SvEngine 10257

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
- al_resample_all (1 resamples all audio using Soxr, with 0.95 passband, to 48 kHz. Can greatly increase audio quality as OpenAL Soft's real-time resampler is not as good)
- al_clamping_mode (when resampling, what to do with samples that overload the pipeline)
  - 0: Clamp
  - 1: Reduce gain of the whole audio
  - 2: Keep as is (allow clipping)

# Console commands
- al_version (shows current MetaAudio version, along with the current OpenAL device and the OpenAL version)
- al_show_basic_devices (shows all basic OpenAL, such as just _OpenAL Soft_)
- al_show_full_devices (shows the full name of all OpenAL devices, such as _Generic Software on Realtek Speakers_)
- al_reset_efx (resets and re-creates all EFX sends, useful for buggy OpenAL drivers that were unable to enable a effect send for some unknown reason)

# Executable arguments
- al_maxsends (sets the max number of simultaneous EFX effects, MetaAudio uses a maximum of 2 but this can limit it to less)
- al_device (sets the OpenAL device to use, can be _OpenAL Soft_ or _OpenAL Soft on Realtek_, for example)

# Installation

1. Install latest version of [MetaHookSv](https://github.com/hzqst/MetaHookSv) .

2. Copy the `metahook` folder to your existing `metahook` dir.

Example folder structure, where <game> can be "valve", "cstrike", "ns", "gearbox" or any other mod:

```
|%STEAM%\steamapps\common\Half-Life\
|----> metahook.exe (metahook_blob.exe, svencoop.exe)
|----> <game>\
  |----> metahook\
    |----> plugins\
      |----> MetaAudio.dll
    |----> dlls\
      |----> sndfile.dll
      |----> OpenAL32.dll (OpenAL software implemention, remove to use pre-installed hardware acceleration version of OpenAL from system)
    |----> configs\
      |----> plugins.lst
```

3. Add a new entry `MetaAudio.dll` in the `plugins.lst`.

* One should start the game through "MetaHook.exe". It is recommended to create a shortcut with at least the following launch options：`-steam -insecure -game <game>`.

* You will have to install the hardware acceleration version of OpenAL from https://www.openal.org/ if you removed the software implemention one from `dlls\`.

# Known bugs

- Some sounds are too quiet
- Some sounds does not use the correct source origin

# Maybe in the future

- Integrate SteamAudio for reflections/reverb in addition to OpenAL's native effects

# Build Requirements

1. [Visual Studio 2022, with vc143 toolset](https://visualstudio.microsoft.com/)

2. [CMake](https://cmake.org/download/)

3. [Git for Windows](https://gitforwindows.org/)

# Compiling

1. `git clone --recursive <repoUrl>`, where `<repoUrl>` should be the MetaAudio repository URL.

2. Run `scripts\build-MetaAudio-x86-Release.bat`

# Debugging

1. `git clone --recursive <repoUrl>`, where `<repoUrl>` should be the MetaAudio repository URL.

2. Run `externals\MetaHookSv\scripts\debug-<GameName>.bat`, where `<GameName>` should be the game you are going to debug with.

3. Run `scripts\build-MetaAudio-x86-Debug.bat`

4. Open `build\x86\Debug\MetaAudio.sln` with Visual Studio IDE, set `MetaAudio` as launch project, press F5 to start debugging.

* Other games follow the same instruction.

* You should restart Visual Studio IDE to apply changes to debugging profile, if Visual Studio IDE was running at the 2nd step.

# Credits

nagist, for MetaHook (https://github.com/nagist/metahook)

hzqst, for the original MetaAudio (https://github.com/hzqst/MetaRenderer)

# Support

Support group available through an XMPP MUC:

xmpp:metaaudio@conference.xmpp.zone?join