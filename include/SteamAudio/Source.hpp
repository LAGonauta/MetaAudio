#pragma once

#include <variant>

#include "SteamAudioLib.h"
#include "ObjectPtr.hpp"

namespace MetaAudio {
	namespace SteamAudio {
		class Simulator;

		class Source final : public ObjectPtr<IPLSource> {
			friend class Simulator;
		public:
			constexpr Source(nullptr_t) noexcept : ObjectPtr(nullptr) {}
			Source(IPLSource ptr) : ObjectPtr(ptr) {};
			void SetInputs(IPLSimulationFlags flags, IPLSimulationInputs& inputs);
			IPLSimulationOutputs GetOutputs(IPLSimulationFlags flags);

		protected:
			// Inherited via ObjectPtr
			void retain(IPLSource handle) override;
			void release(IPLSource handle) override;
		};
	}
}