#include "SteamAudio/Source.hpp"

namespace MetaAudio {
	namespace SteamAudio {
		void Source::SetInputs(IPLSimulationFlags flags, IPLSimulationInputs& inputs)
		{
			gSteamAudio.iplSourceSetInputs(this->m_handle, flags, &inputs);
		}

		IPLSimulationOutputs Source::GetOutputs(IPLSimulationFlags flags)
		{
			IPLSimulationOutputs result{};
			gSteamAudio.iplSourceGetOutputs(this->m_handle, flags, &result);
			return result;
		}

		void Source::retain(IPLSource handle)
		{
			gSteamAudio.iplSourceRetain(handle);
		}

		void Source::release(IPLSource handle)
		{
			gSteamAudio.iplSourceRelease(&handle);
		}
	}
}
