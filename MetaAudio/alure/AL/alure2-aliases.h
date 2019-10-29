/*********
 * Defines aliases for relevant STL containers. Be aware that even though these
 * can be modified to use custom types, the library expects them to have
 * standard APIs and semantics. Changing these aliases will also break ABI, so
 * they should be left alone for shared builds unless all users are also
 * rebuilt.
 */

#ifndef AL_ALURE2_ALIASES_H
#define AL_ALURE2_ALIASES_H

#include <vector>
#include <string>
#include <memory>
#include <future>
#include <chrono>
#include <array>

namespace alure {

// Convenience aliases
template<typename T> using RemoveRefT = typename std::remove_reference<T>::type;
template<bool B, typename T=void> using EnableIfT = typename std::enable_if<B,T>::type;


// NOTE: Need to define this as a macro since we can't use the aliased type
// names for explicit template instantiation, and the whole purpose of these
// aliases is to avoid respecifying the desired implementation.
#define ALURE_SHARED_PTR_TYPE std::shared_ptr


// Duration in seconds, using double precision
using Seconds = std::chrono::duration<double>;

// A SharedPtr implementation, defaults to C++11's std::shared_ptr.
template<typename... Args> using SharedPtr = ALURE_SHARED_PTR_TYPE<Args...>;
template<typename T, typename... Args>
inline SharedPtr<T> MakeShared(Args&&... args)
{ return std::make_shared<T>(std::forward<Args>(args)...); }

// A WeakPtr implementation, defaults to C++11's std::weak_ptr.
template<typename... Args> using WeakPtr = std::weak_ptr<Args...>;

// A UniquePtr implementation, defaults to C++11's std::unique_ptr.
template<typename... Args> using UniquePtr = std::unique_ptr<Args...>;
// Implement MakeUnique for single objects and arrays.
namespace _details {
    template<typename T>
    struct MakeUniq { using object = UniquePtr<T>; };
    template<typename T>
    struct MakeUniq<T[]> { using array = UniquePtr<T[]>; };
    template<typename T, std::size_t N>
    struct MakeUniq<T[N]> { struct invalid_type { }; };
} // namespace _details
// MakeUnique for a single object.
template<typename T, typename... Args>
inline typename _details::MakeUniq<T>::object MakeUnique(Args&&... args)
{ return UniquePtr<T>(new T(std::forward<Args>(args)...)); }
// MakeUnique for an array.
template<typename T>
inline typename _details::MakeUniq<T>::array MakeUnique(std::size_t num)
{ return UniquePtr<T>(new typename std::remove_extent<T>::type[num]()); }
// Disable MakeUnique for an array of declared size.
template<typename T, typename... Args>
inline typename _details::MakeUniq<T>::invalid_type MakeUnique(Args&&...) = delete;

// A Promise/Future (+SharedFuture) implementation, defaults to C++11's
// std::promise, std::future, and std::shared_future.
template<typename... Args> using Promise = std::promise<Args...>;
template<typename... Args> using Future = std::future<Args...>;
template<typename... Args> using SharedFuture = std::shared_future<Args...>;

// A Vector implementation, defaults to C++'s std::vector.
template<typename... Args> using Vector = std::vector<Args...>;

// A static-sized Array implementation, defaults to C++11's std::array.
template<typename T, std::size_t N> using Array = std::array<T, N>;

// A String implementation, default's to C++'s std::string.
template<typename... Args> using BasicString = std::basic_string<Args...>;
using String = BasicString<std::string::value_type>;

} // namespace alure

#endif /* AL_ALURE2_ALIASES_H */
