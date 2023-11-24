#pragma once

#include <stdexcept>

#include "SteamAudioLib.h"

namespace MetaAudio {
	namespace SteamAudio {
		template<class HANDLE>
		class ObjectPtr {
		public:
			constexpr ObjectPtr(nullptr_t) noexcept {}
			virtual ~ObjectPtr() {
				if (m_handle != nullptr) {
					release(m_handle);
				}
			}

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
			virtual void retain(HANDLE handle) {
				throw std::logic_error("retain not implemented");
			}
			virtual void release(HANDLE handle) {
				throw std::logic_error("release not implemented");
			}
			HANDLE m_handle = nullptr;
			ObjectPtr(HANDLE handle) : m_handle(handle) {}
		};
	}
}