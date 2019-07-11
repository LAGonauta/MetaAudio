//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#include <string.h>
#include <stdlib.h>
#include "interface.h"

#ifndef _WIN32  // LINUX
#include <dlfcn.h>
#include <filesystem>
#endif

// ------------------------------------------------------------------------------------ //
// InterfaceReg.
// ------------------------------------------------------------------------------------ //
InterfaceReg *InterfaceReg::s_pInterfaceRegs = NULL;

InterfaceReg::InterfaceReg(InstantiateInterfaceFn fn, const char *pName) :
  m_pName(pName)
{
  m_CreateFn = fn;
  m_pNext = s_pInterfaceRegs;
  s_pInterfaceRegs = this;
}

// ------------------------------------------------------------------------------------ //
// CreateInterface.
// ------------------------------------------------------------------------------------ //
EXPORT_FUNCTION IBaseInterface *CreateInterface(const char *pName, int *pReturnCode)
{
  InterfaceReg *pCur;

  for (pCur = InterfaceReg::s_pInterfaceRegs; pCur; pCur = pCur->m_pNext)
  {
    if (strcmp(pCur->m_pName, pName) == 0)
    {
      if (pReturnCode)
      {
        *pReturnCode = IFACE_OK;
      }
      return pCur->m_CreateFn();
    }
  }

  if (pReturnCode)
  {
    *pReturnCode = IFACE_FAILED;
  }
  return NULL;
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

HINTERFACEMODULE Sys_LoadModule(const char *pModuleName)
{
#ifdef _WIN32
  return (HINTERFACEMODULE)LoadLibrary(pModuleName);
#else  // LINUX
  // Linux dlopen() doesn't look in the current directory for libraries.
  // We tell it to, so people don't have to 'install' libraries as root.
  std::string library_path = std::filesystem::current_path().string() + "/" + pModuleName;

  return (HINTERFACEMODULE)dlopen(library_path.c_str(), RTLD_NOW);
#endif
}

void Sys_FreeModule(HINTERFACEMODULE hModule)
{
  if (!hModule)
    return;
#ifdef _WIN32
  FreeLibrary((HMODULE)hModule);
#else  // LINUX
  dlclose((void *)hModule);
#endif
}

//-----------------------------------------------------------------------------
// Purpose: returns the instance of this module
// Output : interface_instance_t
//-----------------------------------------------------------------------------
CreateInterfaceFn Sys_GetFactoryThis(void)
{
  return CreateInterface;
}

//-----------------------------------------------------------------------------
// Purpose: returns the instance of the named module
// Input  : *pModuleName - name of the module
// Output : interface_instance_t - instance of that module
//-----------------------------------------------------------------------------

CreateInterfaceFn Sys_GetFactory(HINTERFACEMODULE hModule)
{
  if (!hModule)
    return nullptr;
#ifdef _WIN32
  return (CreateInterfaceFn)GetProcAddress((HMODULE)hModule, CREATEINTERFACE_PROCNAME);
#else  // LINUX
  return (CreateInterfaceFn)dlsym((void *)hModule, CREATEINTERFACE_PROCNAME);
#endif
}