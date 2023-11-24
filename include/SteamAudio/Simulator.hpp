#pragma once

#include <variant>

#include "SteamAudioLib.h"
#include "ObjectPtr.hpp"
#include "Scene.hpp"
#include "Source.hpp"

namespace MetaAudio {
	namespace SteamAudio {
		class Simulator final : public ObjectPtr<IPLSimulator> {
		public:
			//constexpr Simulator() : ObjectPtr() {};
			constexpr Simulator(nullptr_t) noexcept : ObjectPtr(nullptr) {}
			Simulator(IPLSimulator ptr) : ObjectPtr(ptr) {};
			std::variant<Source, IPLerror> SourceCreate(IPLSourceSettings& sourceSettings);
			void SourceRemove(const Source& source);
			void SetScene(const Scene& scene);
			void SourceAdd(const Source& source);
			void Commit();

			void SetSharedInputs(IPLSimulationFlags flags, IPLSimulationSharedInputs& inputs);
			void RunDirect();

		protected:
			// Inherited via ObjectPtr
			void retain(IPLSimulator handle) override;
			void release(IPLSimulator handle) override;
		};
	}
}