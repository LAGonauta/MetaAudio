#include "SteamAudio/Scene.hpp"

namespace MetaAudio {
	namespace SteamAudio {
		std::variant<StaticMesh, IPLerror> Scene::StaticMeshCreate(IPLStaticMeshSettings& staticMeshSettings)
		{
			IPLStaticMesh staticMesh(nullptr);
			IPLerror err = gSteamAudio.iplStaticMeshCreate(this->m_handle, &staticMeshSettings, &staticMesh);
			if (err != IPLerror::IPL_STATUS_SUCCESS) {
				return err;
			}

			return std::variant<StaticMesh, IPLerror>(std::in_place_type<StaticMesh>, staticMesh);
		}

		void Scene::StaticMeshAdd(const StaticMesh& staticMesh)
		{
			gSteamAudio.iplStaticMeshAdd(staticMesh.m_handle, this->m_handle);
		}

		void Scene::Commit() {
			gSteamAudio.iplSceneCommit(this->m_handle);
		}
	}
}
