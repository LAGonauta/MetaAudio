#include <windows.h>
#include <memory>
#include <functional>
#include <mutex>

#include "SteamAudioLib.h"

namespace MetaAudio {
	static std::mutex libMutex;
	static HMODULE steamAudioLibraryInstance;

#define SetSteamAudioFunctionPointer(target, library, __function_name__) \
    target.__function_name__ = reinterpret_cast<decltype(MetaAudio::SteamAudioLib::__function_name__)>(GetProcAddress(library, #__function_name__));

	SteamAudioLib SteamAudioLib::Load() {
		std::scoped_lock<std::mutex> lk(libMutex);
		if (steamAudioLibraryInstance == nullptr) {
			steamAudioLibraryInstance = LoadLibrary("phonon.dll");
			if (steamAudioLibraryInstance == nullptr) {
				return SteamAudioLib();
			}
		}

		if (steamAudioLibraryInstance == nullptr) {
			return SteamAudioLib();
		}

		SteamAudioLib ret;
		// TODO: all pointers
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSimulatorCreate);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSimulatorRetain);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSimulatorRelease);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSimulatorSetScene);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSimulatorCommit);

		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSourceCreate);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSourceRetain);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSourceRelease);

		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSceneCreate);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSceneRetain);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSceneRelease);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplSceneCommit);

		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplStaticMeshRelease);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplStaticMeshRetain);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplStaticMeshCreate);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplStaticMeshAdd);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplStaticMeshRemove);

		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplContextCreate);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplContextRelease);
		SetSteamAudioFunctionPointer(ret, steamAudioLibraryInstance, iplContextRetain);
		ret.m_valid = true;

		return ret;
	}

	void SteamAudioLib::Unload()
	{
		std::scoped_lock<std::mutex> lk(libMutex);
		if (steamAudioLibraryInstance != nullptr) {
			FreeLibrary(steamAudioLibraryInstance);
			steamAudioLibraryInstance = nullptr;
		}
	}

	bool SteamAudioLib::IsValid() const {
		return m_valid;
	}
}