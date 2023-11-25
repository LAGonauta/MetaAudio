#include <windows.h>
#include <memory>
#include <functional>
#include <mutex>

#include "SteamAudioLib.h"

namespace MetaAudio {
	static std::mutex libMutex;
	static HMODULE saModule;

#define SetSteamAudioFunctionPointer(target, library, __function_name__) \
    target.__function_name__ = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::__function_name__)>(GetProcAddress(library, #__function_name__));

	SteamAudioLib SteamAudioLib::Load() {
		std::scoped_lock<std::mutex> lk(libMutex);
		if (saModule == nullptr) {
			saModule = LoadLibrary("phonon.dll");
			if (saModule == nullptr) {
				return SteamAudioLib();
			}
		}

		if (saModule == nullptr) {
			return SteamAudioLib();
		}

		SteamAudioLib ret{};
		// TODO: all pointers
		ret.iplContextCreate = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplContextCreate)>(GetProcAddress(saModule, "_iplContextCreate@8"));
		ret.iplContextRelease = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplContextRelease)>(GetProcAddress(saModule, "_iplContextRelease@4"));
		ret.iplContextRetain = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplContextRetain)>(GetProcAddress(saModule, "_iplContextRetain@4"));

		ret.iplSimulatorCreate = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSimulatorCreate)>(GetProcAddress(saModule, "_iplSimulatorCreate@12"));
		ret.iplSimulatorRetain = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSimulatorRetain)>(GetProcAddress(saModule, "_iplSimulatorRetain@4"));
		ret.iplSimulatorRelease = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSimulatorRelease)>(GetProcAddress(saModule, "_iplSimulatorRelease@4"));
		ret.iplSimulatorSetScene = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSimulatorSetScene)>(GetProcAddress(saModule, "_iplSimulatorSetScene@8"));
		ret.iplSimulatorCommit = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSimulatorCommit)>(GetProcAddress(saModule, "_iplSimulatorCommit@4"));
		ret.iplSimulatorSetSharedInputs = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSimulatorSetSharedInputs)>(GetProcAddress(saModule, "_iplSimulatorSetSharedInputs@12"));
		ret.iplSimulatorRunDirect = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSimulatorRunDirect)>(GetProcAddress(saModule, "_iplSimulatorRunDirect@4"));

		ret.iplSourceCreate = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSourceCreate)>(GetProcAddress(saModule, "_iplSourceCreate@12"));
		ret.iplSourceRetain = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSourceRetain)>(GetProcAddress(saModule, "_iplSourceRetain@4"));
		ret.iplSourceRelease = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSourceRelease)>(GetProcAddress(saModule, "_iplSourceRelease@4"));
		ret.iplSourceRemove = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSourceRemove)>(GetProcAddress(saModule, "_iplSourceRemove@8"));
		ret.iplSourceAdd = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSourceAdd)>(GetProcAddress(saModule, "_iplSourceAdd@8"));
		ret.iplSourceSetInputs = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSourceSetInputs)>(GetProcAddress(saModule, "_iplSourceSetInputs@12"));
		ret.iplSourceGetOutputs = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSourceGetOutputs)>(GetProcAddress(saModule, "_iplSourceGetOutputs@12"));

		ret.iplSceneCreate = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSceneCreate)>(GetProcAddress(saModule, "_iplSceneCreate@12"));
		ret.iplSceneRetain = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSceneRetain)>(GetProcAddress(saModule, "_iplSceneRetain@4"));
		ret.iplSceneRelease = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSceneRelease)>(GetProcAddress(saModule, "_iplSceneRelease@4"));
		ret.iplSceneCommit = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplSceneCommit)>(GetProcAddress(saModule, "_iplSceneCommit@4"));

		ret.iplStaticMeshRelease = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplStaticMeshRelease)>(GetProcAddress(saModule, "_iplStaticMeshRelease@4"));
		ret.iplStaticMeshRetain = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplStaticMeshRetain)>(GetProcAddress(saModule, "_iplStaticMeshRetain@4"));
		ret.iplStaticMeshCreate = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplStaticMeshCreate)>(GetProcAddress(saModule, "_iplStaticMeshCreate@12"));
		ret.iplStaticMeshAdd = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplStaticMeshAdd)>(GetProcAddress(saModule, "_iplStaticMeshAdd@8"));
		ret.iplStaticMeshRemove = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::iplStaticMeshRemove)>(GetProcAddress(saModule, "_iplStaticMeshRemove@8"));

		// verify every pointer is valid
		ret.m_valid = true;
		auto num_pointers = sizeof(SteamAudioLib) / sizeof(void*) - 1; // The last field is a boolean, not a pointer. This ensures we won't go over.
		void** pointers_array = reinterpret_cast<void**>(&ret);
		for (auto i = 0; i < num_pointers; ++i) {
			if (pointers_array[i] == nullptr) {
				ret.m_valid = false;
				break;
			}
		}

		return ret;
	}

	void SteamAudioLib::Unload()
	{
		std::scoped_lock<std::mutex> lk(libMutex);
		if (saModule != nullptr) {
			FreeLibrary(saModule);
			saModule = nullptr;
		}
	}

	bool SteamAudioLib::IsValid() const {
		return m_valid;
	}
}