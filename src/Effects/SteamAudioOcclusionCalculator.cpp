#include "Effects/SteamAudioOcclusionCalculator.hpp"
#include "pm_defs.h"
#include "Utilities/VectorUtils.hpp"

namespace MetaAudio
{
	SteamAudioOcclusionCalculator::SteamAudioOcclusionCalculator(std::shared_ptr<SteamAudioMapMeshLoader> meshLoader, const event_api_s& event_api) : meshLoader(meshLoader), event_api(event_api)
	{
	}

	void SteamAudioOcclusionCalculator::PlayerTrace(Vector3* start, Vector3* end, pmtrace_s& tr)
	{
		// 0 = regular player hull, 1 = ducked player hull, 2 = point hull
		event_api.EV_SetTraceHull(2);
		event_api.EV_PlayerTrace(reinterpret_cast<float*>(start), reinterpret_cast<float*>(end), PM_STUDIO_IGNORE, -1, &tr);
	}

	OcclusionFrequencyGain SteamAudioOcclusionCalculator::GetParameters(
		Vector3 listenerPosition,
		Vector3 listenerAhead,
		Vector3 listenerUp,
		Vector3 audioSourcePosition,
		float sourceRadius,
		float attenuationMultiplier
	)
	{
		if (attenuationMultiplier == ATTN_NONE)
		{
			return OcclusionFrequencyGain{ 1.0f, 1.0f, 1.0f };
		}

		auto toIPLVector3 = [](Vector3* vector) { return *reinterpret_cast<IPLVector3*>(vector); };

		auto simulator = meshLoader->CurrentSimulator();
		IPLSourceSettings sourceSettings{};
		sourceSettings.flags = IPLSimulationFlags::IPL_SIMULATIONFLAGS_DIRECT;
		auto sourceResult = simulator.SourceCreate(sourceSettings);
		if (std::holds_alternative<IPLerror>(sourceResult) && std::get<1>(sourceResult) != IPLerror::IPL_STATUS_SUCCESS) {
			// TODO: LOG
			return OcclusionFrequencyGain{ 1.0f, 1.0f, 1.0f };
		}
		auto& source = std::get<0>(sourceResult);

		simulator.SourceAdd(source);
		simulator.Commit();

		IPLSimulationInputs sourceInputs{};
		sourceInputs.flags = static_cast<IPLSimulationFlags>(
			IPLSimulationFlags::IPL_SIMULATIONFLAGS_DIRECT |
			IPLSimulationFlags::IPL_SIMULATIONFLAGS_REFLECTIONS);
		sourceInputs.directFlags = static_cast<IPLDirectSimulationFlags>(
			IPLDirectSimulationFlags::IPL_DIRECTSIMULATIONFLAGS_OCCLUSION |
			IPLDirectSimulationFlags::IPL_DIRECTSIMULATIONFLAGS_AIRABSORPTION |
			IPLDirectSimulationFlags::IPL_DIRECTSIMULATIONFLAGS_DISTANCEATTENUATION |
			IPLDirectSimulationFlags::IPL_DIRECTSIMULATIONFLAGS_TRANSMISSION);
		sourceInputs.reverbScale[0] = 1.0f;
		sourceInputs.reverbScale[1] = 1.0f;
		sourceInputs.reverbScale[2] = 1.0f;
		sourceInputs.numTransmissionRays = SteamAudioOcclusionCalculator::NUMBER_OCCLUSION_RAYS;
		sourceInputs.numOcclusionSamples = SteamAudioOcclusionCalculator::NUMBER_OCCLUSION_RAYS;
		sourceInputs.source = {
			{1,0,0},
			{0,1,0},
			{0,0,1},
			toIPLVector3(&audioSourcePosition)
		};
		sourceInputs.occlusionRadius = sourceRadius;
		sourceInputs.occlusionType = IPLOcclusionType::IPL_OCCLUSIONTYPE_VOLUMETRIC;
		source.SetInputs(sourceInputs.flags, sourceInputs);

		IPLSimulationSharedInputs simulationSharedInputs{};
		simulationSharedInputs.listener = {
			{1,0,0},
			toIPLVector3(&listenerUp),
			toIPLVector3(&listenerAhead),
			toIPLVector3(&listenerPosition)
		};
		simulationSharedInputs.numRays = 4096;
		simulationSharedInputs.numBounces = 16;
		simulationSharedInputs.duration = 2.0f;
		simulationSharedInputs.order = 1;
		simulationSharedInputs.irradianceMinDistance = 1.0f;
		simulator.SetSharedInputs(sourceInputs.flags, simulationSharedInputs);
		simulator.RunDirect();
		//simulator.RunReflections();

		auto result = source.GetOutputs(sourceInputs.flags);
		simulator.SourceRemove(source);
		simulator.Commit();

		if (result.direct.occlusion < 0.5f) //more than half occluded, add transmission component based on obstruction length
		{
			pmtrace_s tr;

			// Convert to GoldSrc coordinates system
			auto listener_position = GoldSrc_UnpackVector(listenerPosition);
			auto audio_source_position = GoldSrc_UnpackVector(audioSourcePosition);

			// set up traceline from player eyes to sound emitting entity origin
			PlayerTrace(&listener_position, &audio_source_position, tr);

			// If hit, traceline between ent and player to get solid length.
			if ((tr.fraction < 1.0f || tr.allsolid || tr.startsolid) && tr.fraction < 0.99f)
			{
				alure::Vector3 obstruction_first_point(tr.endpos);
				PlayerTrace(&audio_source_position, &listener_position, tr);

				if ((tr.fraction < 1.0f || tr.allsolid || tr.startsolid) && tr.fraction < 0.99f && !tr.startsolid)
				{
					auto distance = obstruction_first_point.getDistance(tr.endpos) * AL_UnitToMeters;
					result.direct.transmission[0] = alure::dBToLinear(-distance * result.direct.transmission[0]);
					result.direct.transmission[1] = alure::dBToLinear(-distance * result.direct.transmission[1]);
					result.direct.transmission[2] = alure::dBToLinear(-distance * result.direct.transmission[2]);
				}
			}
		}

		auto getFactor = [&](size_t index) { return std::clamp((result.direct.occlusion + (1 - result.direct.occlusion) * result.direct.transmission[index]) / attenuationMultiplier, 0.0f, 1.0f); };
		auto ret = OcclusionFrequencyGain{ getFactor(0), getFactor(1), getFactor(2) };
		return ret;
	}
}