#include "SteamAudio/StaticMesh.hpp"

namespace MetaAudio {
	namespace SteamAudio {
		void StaticMesh::retain(IPLStaticMesh handle)
		{
			gSteamAudio.iplStaticMeshRetain(handle);
		}

		void StaticMesh::release(IPLStaticMesh handle)
		{
			gSteamAudio.iplStaticMeshRelease(&handle);
		}
	}
}
