#include <metahook.h>
#include <r_studioint.h>

#include "exportfuncs.h"
#include "plugins.h"
#include "snd_local.h"

//Error when can't find sig
void Sys_ErrorEx(const char *fmt, ...)
{
  va_list argptr;
  char msg[1024];

  va_start(argptr, fmt);
  _vsnprintf_s(msg, sizeof(msg), fmt, argptr);
  va_end(argptr);

  if (gEngfuncs.pfnClientCmd)
    gEngfuncs.pfnClientCmd("escape\n");

  MessageBox(NULL, msg, "Fatal Error", MB_ICONERROR);
  TerminateProcess((HANDLE)-1, 0);
}

cl_exportfuncs_t gClientfuncs ={ 0 };

cl_enginefunc_t gEngfuncs = { 0 };