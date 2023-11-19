#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#ifndef NULL
#define NULL 0
#endif

#include "interface.h"
#include "IFileSystem.h"
#include <metahook.h>

extern IFileSystem *g_pFileSystem;
extern IFileSystem_HL25 *g_pFileSystem_HL25;
extern IFileSystem *g_pFullFileSystem;

#endif