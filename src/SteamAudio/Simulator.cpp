#include "SteamAudio/Simulator.hpp"

namespace MetaAudio {
	namespace SteamAudio {
		std::variant<Source, IPLerror> Simulator::SourceCreate(IPLSourceSettings& sourceSettings)
		{
			IPLSource source(nullptr);
			IPLerror err = gSteamAudio.iplSourceCreate(this->m_handle, &sourceSettings, &source);
			if (err != IPLerror::IPL_STATUS_SUCCESS) {
				return err;
			}

			return std::variant<Source, IPLerror>(std::in_place_type<Source>, source);
		}

		void Simulator::SourceRemove(const Source& source)
		{
			gSteamAudio.iplSourceRemove(source.m_handle, this->m_handle);
		}

		void Simulator::SourceAdd(const Source& source)
		{
			gSteamAudio.iplSourceAdd(source.m_handle, this->m_handle);
		}

		void Simulator::SetScene(const Scene& scene) {
			gSteamAudio.iplSimulatorSetScene(this->m_handle, scene.m_handle);
		}

		void Simulator::Commit() {
			gSteamAudio.iplSimulatorCommit(this->m_handle);
		}

		void Simulator::SetSharedInputs(IPLSimulationFlags flags, IPLSimulationSharedInputs& inputs) {
			gSteamAudio.iplSimulatorSetSharedInputs(this->m_handle, flags, &inputs);
		}

		void Simulator::RunDirect() {
			gSteamAudio.iplSimulatorRunDirect(this->m_handle);
		}

		void Simulator::RunReflections() {
			gSteamAudio.iplSimulatorRunReflections(this->m_handle);
		}
	}
}
