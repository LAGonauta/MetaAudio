#pragma once
#include <unordered_map>

#include "alure2.h"
#include "SteamAudio/Context.hpp"
#include "SteamAudio/Scene.hpp"
#include "SteamAudio/Simulator.hpp"

namespace MetaAudio
{
	class SteamAudioMapMeshLoader final
	{
	private:
		class ProcessedMap final
		{
		private:
			SteamAudio::Scene scene = nullptr;
			SteamAudio::StaticMesh static_mesh = nullptr;
			SteamAudio::Simulator simulator = nullptr;
			std::string mapName;

		public:
			ProcessedMap(const std::string& mapName, SteamAudio::Scene scene, SteamAudio::StaticMesh mesh, SteamAudio::Simulator simulator)
				: scene(scene), static_mesh(mesh), simulator(simulator), mapName(mapName)
			{
			}

			const std::string& Name()
			{
				return mapName;
			}

			SteamAudio::Simulator Simulator() {
				return simulator;
			}
		};

		IPLSimulationSettings sa_simul_settings;
		SteamAudio::Context sa_context;

		std::unique_ptr<ProcessedMap> current_map = nullptr;

		alure::Vector3 Normalize(const alure::Vector3& vector);
		float DotProduct(const alure::Vector3& left, const alure::Vector3& right);

		// Transmission details:
		// SteamAudio returns the transmission property of the material that was hit, not how much was transmitted
		// We should calculate ourselves how much is actually transmitted. The unit used in MetaAudio is actually
		// the attenuation `dB/m`, not how much is transmitted per meter. 
		std::array<IPLMaterial, 1> materials{ {0.10f, 0.20f, 0.30f, 0.05f, 0.100f, 0.050f, 0.030f} };
	public:
		SteamAudioMapMeshLoader(SteamAudio::Context sa_context, IPLSimulationSettings simulSettings);

		// Checks if map is current, if not update it
		void update();

		SteamAudio::Simulator CurrentSimulator();
	};
}