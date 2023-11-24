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
			//constexpr StaticMesh() : ObjectPtr() {};
			constexpr StaticMesh(nullptr_t) noexcept : ObjectPtr(nullptr) {}
			StaticMesh(IPLStaticMesh ptr) : ObjectPtr(ptr) {};

		protected:
			// Inherited via ObjectPtr
			void retain(IPLStaticMesh handle) override;
			void release(IPLStaticMesh handle) override;
		};
	}
}