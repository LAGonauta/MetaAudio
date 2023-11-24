#include <iostream>

#include "SteamAudio/Context.hpp"

namespace MetaAudio {
	namespace SteamAudio {
		std::variant<Context, IPLerror> Context::Create(IPLContextSettings& contextSettings) {
			IPLContext ctx(nullptr);
			IPLerror err = gSteamAudio.iplContextCreate(&contextSettings, &ctx);
			if (err != IPLerror::IPL_STATUS_SUCCESS) {
				return err;
			}

			return std::variant<Context, IPLerror>(std::in_place_type<Context>, ctx);
		}

		// try IPL_SCENETYPE_RADEONRAYS, then IPL_SCENETYPE_EMBREE, and then finally IPL_SCENETYPE_DEFAULT
		std::variant<Simulator, IPLerror> Context::CreateSimulator(IPLSimulationSettings &simulationSettings)
		{
			IPLSimulator simulator(nullptr);
			IPLerror err = gSteamAudio.iplSimulatorCreate(this->m_handle, &simulationSettings, &simulator);
			if (err != IPLerror::IPL_STATUS_SUCCESS) {
				return err;
			}

			return std::variant<Simulator, IPLerror>(std::in_place_type<Simulator>, simulator);
		}

		std::variant<Scene, IPLerror> Context::CreateScene(IPLSceneSettings& sceneSettings)
		{
			IPLScene scene(nullptr);
			IPLerror err = gSteamAudio.iplSceneCreate(this->m_handle, &sceneSettings, &scene);
			if (err != IPLerror::IPL_STATUS_SUCCESS) {
				return err;
			}

			return std::variant<Scene, IPLerror>(std::in_place_type<Scene>, scene);
		}
	}
}