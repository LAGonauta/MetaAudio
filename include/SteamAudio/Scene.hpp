#pragma once

#include <variant>
#include <filesystem>

#include "SteamAudioLib.h"
#include "ObjectPtr.hpp"
#include "StaticMesh.hpp"

namespace MetaAudio {
	namespace SteamAudio {
		class Simulator;
		
		class Scene final : public ObjectPtr<IPLScene> {
			friend class Simulator;
		public:
			//constexpr Scene() : ObjectPtr() {};
			constexpr Scene(nullptr_t) noexcept : ObjectPtr(nullptr) {}
			Scene(IPLScene ptr) : ObjectPtr(ptr) {};
			std::variant<StaticMesh, IPLerror> StaticMeshCreate(IPLStaticMeshSettings& staticMeshSettings);
			void StaticMeshAdd(const StaticMesh& staticMesh);

			void Commit();
			void SaveOBJ(const std::filesystem::path& path) const;
		};
	}
}