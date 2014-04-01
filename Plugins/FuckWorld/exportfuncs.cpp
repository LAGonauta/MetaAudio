#include <metahook.h>

cl_enginefunc_t gEngfuncs;

int Initialize(struct cl_enginefuncs_s *pEnginefuncs, int iVersion)
{
	memcpy(&gEngfuncs, pEnginefuncs, sizeof(gEngfuncs));
	return gExportfuncs.Initialize(pEnginefuncs, iVersion);
}

void HUD_Init(void)
{
	return gExportfuncs.HUD_Init();
}

int HUD_Redraw(float time, int intermission)
{
	static int count = 0;

	if (count < 10)
	{
		gEngfuncs.Con_Printf("Fuck World!!!!! (hit:%d)\n", count + 1);
		count++;
	}

	return gExportfuncs.HUD_Redraw(time, intermission);
}