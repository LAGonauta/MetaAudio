#include <metahook.h>

#include "snd_local.h"
#include "Utilities/AudioCache.hpp"
#include "Loaders/SoundLoader.hpp"
#include "Vox/VoxManager.hpp"
#include "AudioEngine.hpp"

MetaAudio::SteamAudio gSteamAudio;
static std::shared_ptr<MetaAudio::SoundLoader> sound_loader;
static std::unique_ptr<MetaAudio::AudioEngine> audio_engine;

static void AL_Version() { audio_engine->AL_Version(); }
static void AL_ResetEFX() { audio_engine->AL_ResetEFX(); }
static void AL_BasicDevices() { audio_engine->AL_Devices(true); }
static void AL_FullDevices() { audio_engine->AL_Devices(false); }

cl_exportfuncs_t gExportfuncs = {0};
mh_interface_t *g_pInterface = nullptr;
metahook_api_t *g_pMetaHookAPI = nullptr;
mh_enginesave_t *g_pMetaSave = nullptr;
IFileSystem *g_pFileSystem = nullptr;
IFileSystem_HL25* g_pFileSystem_HL25 = nullptr;

HINSTANCE g_hInstance = nullptr, g_hThisModule = nullptr, g_hEngineModule = nullptr, g_hSteamAudioInstance = nullptr;
PVOID g_dwEngineBase = 0;
DWORD g_dwEngineSize = 0;
PVOID g_dwEngineTextBase = 0;
DWORD g_dwEngineTextSize = 0;
PVOID g_dwEngineDataBase = 0;
DWORD g_dwEngineDataSize = 0;
PVOID g_dwEngineRdataBase = 0;
DWORD g_dwEngineRdataSize = 0;
DWORD g_dwEngineBuildnum = 0;
int g_iEngineType = 0;

ICommandLine *CommandLine()
{
  return g_pInterface->CommandLine;
}

void IPlugins::Init(metahook_api_t *pAPI, mh_interface_t *pInterface, mh_enginesave_t *pSave)
{
  g_pInterface = pInterface;
  g_pMetaHookAPI = pAPI;
  g_pMetaSave = pSave;
  g_hInstance = GetModuleHandle(NULL);

  g_hSteamAudioInstance = LoadLibrary("phonon.dll");
  if (g_hSteamAudioInstance)
  {
    SetSteamAudioFunctionPointer(gSteamAudio, g_hSteamAudioInstance, iplCleanup);
    SetSteamAudioFunctionPointer(gSteamAudio, g_hSteamAudioInstance, iplDestroyEnvironment);
    SetSteamAudioFunctionPointer(gSteamAudio, g_hSteamAudioInstance, iplDestroyScene);
    SetSteamAudioFunctionPointer(gSteamAudio, g_hSteamAudioInstance, iplDestroyStaticMesh);
    SetSteamAudioFunctionPointer(gSteamAudio, g_hSteamAudioInstance, iplGetDirectSoundPath);
    SetSteamAudioFunctionPointer(gSteamAudio, g_hSteamAudioInstance, iplCreateScene);
    SetSteamAudioFunctionPointer(gSteamAudio, g_hSteamAudioInstance, iplCreateStaticMesh);
    SetSteamAudioFunctionPointer(gSteamAudio, g_hSteamAudioInstance, iplCreateEnvironment);
    SetSteamAudioFunctionPointer(gSteamAudio, g_hSteamAudioInstance, iplCreateContext);
    SetSteamAudioFunctionPointer(gSteamAudio, g_hSteamAudioInstance, iplDestroyContext);
  }

  auto audio_cache = std::make_shared<MetaAudio::AudioCache>();
  sound_loader = std::make_shared<MetaAudio::SoundLoader>(audio_cache);
  audio_engine = std::make_unique<MetaAudio::AudioEngine>(audio_cache, sound_loader);
}

void IPlugins::Shutdown()
{
  sound_loader.reset();
  audio_engine.reset();
  if (g_hSteamAudioInstance)
  {
    FreeLibrary(g_hSteamAudioInstance);
    g_hSteamAudioInstance = nullptr;
  }
}

void IPlugins::LoadEngine()
{
  g_pFileSystem = g_pInterface->FileSystem;
  if (!g_pFileSystem)//backward compatibility
      g_pFileSystem_HL25 = g_pInterface->FileSystem_HL25;

  g_iEngineType = g_pMetaHookAPI->GetEngineType();
  g_dwEngineBuildnum = g_pMetaHookAPI->GetEngineBuildnum();
  g_hEngineModule = g_pMetaHookAPI->GetEngineModule();
  g_dwEngineBase = g_pMetaHookAPI->GetEngineBase();
  g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();
  g_dwEngineTextBase = g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".text\x0\x0\x0", &g_dwEngineTextSize);
  g_dwEngineDataBase = g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".data\x0\x0\x0", &g_dwEngineDataSize);
  g_dwEngineRdataBase = g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".rdata\x0\x0", &g_dwEngineRdataSize);

  S_FillAddress();
  S_InstallHook(audio_engine.get(), sound_loader.get());
}

void IPlugins::LoadClient(cl_exportfuncs_t *pExportFunc)
{
  memcpy(&gExportfuncs, pExportFunc, sizeof(gExportfuncs));
  memcpy(&gEngfuncs, g_pMetaSave->pEngineFuncs, sizeof(gEngfuncs));

  gEngfuncs.pfnAddCommand("al_version", AL_Version);
  gEngfuncs.pfnAddCommand("al_reset_efx", AL_ResetEFX);
  gEngfuncs.pfnAddCommand("al_show_basic_devices", AL_BasicDevices);
  gEngfuncs.pfnAddCommand("al_show_full_devices", AL_FullDevices);
}

void IPlugins::ExitGame(int iResult)
{
    S_UninstallHook();
    Shutdown();
}

EXPOSE_SINGLE_INTERFACE(IPlugins, IPlugins, METAHOOK_PLUGIN_API_VERSION);