#pragma once

#include <variant>

#include "SteamAudioLib.h"
#include "ObjectPtr.hpp"
#include "Simulator.hpp"
#include "Scene.hpp"

namespace MetaAudio {
	namespace SteamAudio {
		class Context final : public ObjectPtr<IPLContext> {
		public:
			static std::variant<Context, IPLerror> Create(IPLContextSettings& settings);
			constexpr Context(nullptr_t) noexcept : ObjectPtr(nullptr) {}
			Context(IPLContext ptr) : ObjectPtr(ptr) {};

			std::variant<Simulator, IPLerror> CreateSimulator(IPLSimulationSettings& simulationSettings);
			std::variant<Scene, IPLerror> CreateScene(IPLSceneSettings& sceneSettings);

		protected:
			// Inherited via ObjectPtr
			void retain(IPLContext handle) override;
			void release(IPLContext handle) override;
		};
	}
}