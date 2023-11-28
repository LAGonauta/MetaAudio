#include <metahook.h>

#include "snd_local.h"
#include "Utilities/AudioCache.hpp"
#include "Loaders/SoundLoader.hpp"
#include "Vox/VoxManager.hpp"
#include "AudioEngine.hpp"
#include "SteamAudioLib.h"

MetaAudio::SteamAudioLib gSteamAudio;
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

HINSTANCE g_hInstance = nullptr, g_hThisModule = nullptr, g_hEngineModule = nullptr;
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

void IPluginsV4::Init(metahook_api_t *pAPI, mh_interface_t *pInterface, mh_enginesave_t *pSave)
{
  g_pInterface = pInterface;
  g_pMetaHookAPI = pAPI;
  g_pMetaSave = pSave;
  g_hInstance = GetModuleHandle(NULL);

  gSteamAudio = MetaAudio::SteamAudioLib::Load();
  auto audio_cache = std::make_shared<MetaAudio::AudioCache>();
  sound_loader = std::make_shared<MetaAudio::SoundLoader>(audio_cache);
  audio_engine = std::make_unique<MetaAudio::AudioEngine>(audio_cache, sound_loader);
}

void IPluginsV4::Shutdown()
{
  sound_loader.reset();
  audio_engine.reset();
  gSteamAudio = MetaAudio::SteamAudioLib();
  MetaAudio::SteamAudioLib::Unload();
}

void IPluginsV4::LoadEngine(cl_enginefunc_t* pEngfuncs)
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

  memcpy(&gEngfuncs, pEngfuncs, sizeof(gEngfuncs));

  S_FillAddress();
  S_InstallHook(audio_engine.get(), sound_loader.get());
}

void IPluginsV4::LoadClient(cl_exportfuncs_t *pExportFunc)
{
  memcpy(&gExportfuncs, pExportFunc, sizeof(gExportfuncs));

  gEngfuncs.pfnAddCommand("al_version", AL_Version);
  gEngfuncs.pfnAddCommand("al_reset_efx", AL_ResetEFX);
  gEngfuncs.pfnAddCommand("al_show_basic_devices", AL_BasicDevices);
  gEngfuncs.pfnAddCommand("al_show_full_devices", AL_FullDevices);
}

void IPluginsV4::ExitGame(int iResult)
{
    S_UninstallHook();
    Shutdown();
}

const char completeVersion[] =
{
    BUILD_YEAR_CH0, BUILD_YEAR_CH1, BUILD_YEAR_CH2, BUILD_YEAR_CH3,
    '-',
    BUILD_MONTH_CH0, BUILD_MONTH_CH1,
    '-',
    BUILD_DAY_CH0, BUILD_DAY_CH1,
    'T',
    BUILD_HOUR_CH0, BUILD_HOUR_CH1,
    ':',
    BUILD_MIN_CH0, BUILD_MIN_CH1,
    ':',
    BUILD_SEC_CH0, BUILD_SEC_CH1,
    '\0'
};

const char* IPluginsV4::GetVersion(void)
{
    return completeVersion;
}

EXPOSE_SINGLE_INTERFACE(IPluginsV4, IPluginsV4, METAHOOK_PLUGIN_API_VERSION_V4);