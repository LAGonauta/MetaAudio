#include <metahook.h>
#include <IAudio.h>

#include "snd_local.h"
#include "zone.h"

cl_exportfuncs_t gExportfuncs;
mh_interface_t *g_pInterface;
metahook_api_t *g_pMetaHookAPI;
mh_enginesave_t *g_pMetaSave;
IFileSystem *g_pFileSystem;

extern aud_export_t gAudExports;

HINSTANCE g_hInstance, g_hThisModule, g_hEngineModule;
DWORD g_dwEngineBase, g_dwEngineSize;
DWORD g_dwEngineBuildnum;
DWORD g_iVideoMode;
int g_iVideoWidth, g_iVideoHeight, g_iBPP;

#pragma pack(1)
bool g_bWindowed;
bool g_bIsNewEngine;
bool g_bIsUseSteam;
bool g_bIsDebuggerPresent;
#pragma pack()

ICommandLine *CommandLine(void)
{
  return g_pInterface->CommandLine;
}

void IPlugins::Init(metahook_api_t *pAPI, mh_interface_t *pInterface, mh_enginesave_t *pSave)
{
  BOOL(*IsDebuggerPresent)(void) = (BOOL(*)(void))GetProcAddress(GetModuleHandle("kernel32.dll"), "IsDebuggerPresent");

  g_pInterface = pInterface;
  g_pMetaHookAPI = pAPI;
  g_pMetaSave = pSave;
  g_hInstance = GetModuleHandle(NULL);
  g_bIsDebuggerPresent = IsDebuggerPresent() != FALSE;
}

void IPlugins::Shutdown(void)
{
  S_ShutdownAL();
}

void IPlugins::LoadEngine(void)
{
  g_pFileSystem = g_pInterface->FileSystem;
  g_iVideoMode = g_pMetaHookAPI->GetVideoMode(&g_iVideoWidth, &g_iVideoHeight, &g_iBPP, &g_bWindowed);
  g_dwEngineBuildnum = g_pMetaHookAPI->GetEngineBuildnum();

  g_hEngineModule = g_pMetaHookAPI->GetEngineModule();
  g_dwEngineBase = g_pMetaHookAPI->GetEngineBase();
  g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();

  S_FillAddress();
  S_InstallHook();
}

void IPlugins::LoadClient(cl_exportfuncs_t *pExportFunc)
{
  memcpy(&gExportfuncs, pExportFunc, sizeof(gExportfuncs));
  memcpy(&gEngfuncs, g_pMetaSave->pEngineFuncs, sizeof(gEngfuncs));
}

void IPlugins::ExitGame(int iResult)
{
  //force to shutdown OpenAL
  S_ShutdownAL();
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