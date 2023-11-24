#pragma once

#include <stdexcept>
#include <cassert>

#include "SteamAudioLib.h"

namespace MetaAudio {
	namespace SteamAudio {
		template<typename T> struct IsSupported : std::false_type { };
		template<> struct IsSupported<IPLContext> : std::true_type { };
		template<> struct IsSupported<IPLSimulator> : std::true_type { };
		template<> struct IsSupported<IPLSource> : std::true_type { };
		template<> struct IsSupported<IPLScene> : std::true_type { };
		template<> struct IsSupported<IPLStaticMesh> : std::true_type { };

		template<class HANDLE>
		class ObjectPtr {
			static_assert(IsSupported<HANDLE>::value, "Invalid type for ObjectPtr");
		public:
			virtual ~ObjectPtr() {}

			ObjectPtr(const ObjectPtr& other) : ObjectPtr(other.m_handle) {
				if (m_handle != nullptr) {
					retain(m_handle);
				}
			}
			ObjectPtr& operator=(const ObjectPtr& other) {
				if (this == &other) {
					return *this;
				}

				if (this->m_handle == other.m_handle) {
					return *this;
				}
				retain(other.m_handle);
				release(this->m_handle);
				this->m_handle = other.m_handle;
				return *this;
			}

			ObjectPtr(ObjectPtr&& other) : m_handle(std::exchange(other.m_handle, nullptr)) noexcept {}
			ObjectPtr& operator=(ObjectPtr&& other) noexcept {
				std::swap(m_handle, other.m_handle);
				return *this;
			}

			bool operator==(ObjectPtr const& other) {
				return this->m_handle == other.m_handle;
			}

			bool operator!=(ObjectPtr const& other) {
				return this->m_handle != other.m_handle;
			}

		protected:
			HANDLE m_handle = nullptr;
			ObjectPtr(HANDLE handle) : m_handle(handle) {}
			constexpr ObjectPtr(nullptr_t) noexcept {}

		private:
			void retain(HANDLE handle) {
				if (handle != nullptr) {
					bool executed = false;
					if constexpr (std::is_same_v<HANDLE, IPLContext>) {
						gSteamAudio.iplContextRetain(handle);
						executed = true;
					}
					else if constexpr (std::is_same_v<HANDLE, IPLSimulator>) {
						gSteamAudio.iplSimulatorRetain(handle);
						executed = true;
					}
					else if constexpr (std::is_same_v<HANDLE, IPLScene>) {
						gSteamAudio.iplSceneRetain(handle);
						executed = true;
					}
					else if constexpr (std::is_same_v<HANDLE, IPLSource>) {
						gSteamAudio.iplSourceRetain(handle);
						executed = true;
					}
					else if constexpr (std::is_same_v<HANDLE, IPLStaticMesh>) {
						gSteamAudio.iplStaticMeshRetain(handle);
						executed = true;
					}

					assert(executed);
				}
			}

			void release(HANDLE handle) {
				if (handle != nullptr) {
					bool executed = false;
					if constexpr (std::is_same_v<HANDLE, IPLContext>) {
						gSteamAudio.iplContextRelease(&handle);
						executed = true;
					}
					else if constexpr (std::is_same_v<HANDLE, IPLSimulator>) {
						gSteamAudio.iplSimulatorRelease(&handle);
						executed = true;
					}
					else if constexpr (std::is_same_v<HANDLE, IPLScene>) {
						gSteamAudio.iplSceneRelease(&handle);
						executed = true;
					}
					else if constexpr (std::is_same_v<HANDLE, IPLSource>) {
						gSteamAudio.iplSourceRelease(&handle);
						executed = true;
					}
					else if constexpr (std::is_same_v<HANDLE, IPLStaticMesh>) {
						gSteamAudio.iplStaticMeshRelease(&handle);
						executed = true;
					}
					assert(executed);
				}
			}
		};
	}
}