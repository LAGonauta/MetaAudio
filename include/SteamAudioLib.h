#pragma once

extern "C" {
#include "phonon.h"
}

namespace MetaAudio
{
#pragma pack(push, 1)
	class SteamAudioLib
	{
	public:
		decltype(&iplSimulatorCreate) iplSimulatorCreate;
		decltype(&iplSimulatorRetain) iplSimulatorRetain;
		decltype(&iplSimulatorRelease) iplSimulatorRelease;
		decltype(&iplSimulatorSetScene) iplSimulatorSetScene;
		decltype(&iplSimulatorCommit) iplSimulatorCommit;
		decltype(&iplSimulatorSetSharedInputs) iplSimulatorSetSharedInputs;
		decltype(&iplSimulatorRunDirect) iplSimulatorRunDirect;
		decltype(&iplSimulatorRunReflections) iplSimulatorRunReflections;

		decltype(&iplSourceCreate) iplSourceCreate;
		decltype(&iplSourceRetain) iplSourceRetain;
		decltype(&iplSourceRelease) iplSourceRelease;
		decltype(&iplSourceRemove) iplSourceRemove;
		decltype(&iplSourceAdd) iplSourceAdd;
		decltype(&iplSourceSetInputs) iplSourceSetInputs;
		decltype(&iplSourceGetOutputs) iplSourceGetOutputs;

		decltype(&iplSceneCreate) iplSceneCreate;
		decltype(&iplSceneRetain) iplSceneRetain;
		decltype(&iplSceneRelease) iplSceneRelease;
		decltype(&iplSceneCommit) iplSceneCommit;
		decltype(&iplSceneSaveOBJ) iplSceneSaveOBJ;

		decltype(&iplStaticMeshRelease) iplStaticMeshRelease;
		decltype(&iplStaticMeshRetain) iplStaticMeshRetain;
		decltype(&iplStaticMeshCreate) iplStaticMeshCreate;
		decltype(&iplStaticMeshAdd) iplStaticMeshAdd;
		decltype(&iplStaticMeshRemove) iplStaticMeshRemove;

		decltype(&iplContextCreate) iplContextCreate;
		decltype(&iplContextRelease) iplContextRelease;
		decltype(&iplContextRetain) iplContextRetain;

		static SteamAudioLib Load();
		static void Unload();

		bool IsValid() const;
	private:
		bool m_valid{ false };
	};
#pragma pack(pop)
}

extern MetaAudio::SteamAudioLib gSteamAudio;