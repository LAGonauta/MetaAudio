#include <metahook.h>
#include <IAudio.h>

#include "snd_local.h"
#include "Utilities/AudioCache.hpp"
#include "Loaders/SoundLoader.hpp"
#include "Vox/VoxManager.hpp"
#include "AudioEngine.hpp"
#include "DynamicSteamAudio.hpp"
#include "Config/SettingsManager.hpp"

MetaAudio::SettingsManager settings;
static std::shared_ptr<MetaAudio::SoundLoader> sound_loader;
static std::unique_ptr<MetaAudio::AudioEngine> audio_engine;

static void AL_Version() { audio_engine->AL_Version(); }
static void AL_ResetEFX() { audio_engine->AL_ResetEFX(); }
static void AL_BasicDevices() { audio_engine->AL_Devices(true); }
static void AL_FullDevices() { audio_engine->AL_Devices(false); }

cl_exportfuncs_t gExportfuncs;
mh_interface_t *g_pInterface;
metahook_api_t *g_pMetaHookAPI;
mh_enginesave_t *g_pMetaSave;
IFileSystem *g_pFileSystem;

extern aud_export_t gAudExports;

HINSTANCE g_hInstance, g_hThisModule, g_hSteamAudioInstance;
DWORD g_dwEngineBase, g_dwEngineSize;
DWORD g_dwEngineBuildnum;

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
  g_dwEngineBuildnum = g_pMetaHookAPI->GetEngineBuildnum();

  g_dwEngineBase = g_pMetaHookAPI->GetEngineBase();
  g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();

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
  Shutdown();
}

EXPOSE_SINGLE_INTERFACE(IPlugins, IPlugins, METAHOOK_PLUGIN_API_VERSION);

//renderer exports

void IAudio::GetInterface(aud_export_t *pAudExports, const char *version)
{
  if (!strcmp(version, META_AUDIO_VERSION))
  {
    memcpy(pAudExports, &gAudExports, sizeof(aud_export_t));
  }
  else
  {
    Sys_ErrorEx("Meta Audio interface version (%s) should be (%s)\n", version, META_AUDIO_VERSION);
  }
}

EXPOSE_SINGLE_INTERFACE(IAudio, IAudio, AUDIO_API_VERSION);