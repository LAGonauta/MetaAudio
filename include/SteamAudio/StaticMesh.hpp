#pragma once

#include <variant>

#include "SteamAudioLib.h"
#include "ObjectPtr.hpp"

namespace MetaAudio {
	namespace SteamAudio {
		class Scene;

		class StaticMesh final : public ObjectPtr<IPLStaticMesh> {
			friend class Scene;
		public:
			constexpr StaticMesh(nullptr_t) noexcept : ObjectPtr(nullptr) {}
			StaticMesh(IPLStaticMesh ptr) : ObjectPtr(ptr) {};
		};
	}
}