#ifndef AL_ALURE2_H
#define AL_ALURE2_H

#include <type_traits>
#include <utility>
#include <tuple>
#include <cmath>

#include "../alc.h"
#include "../al.h"
#include "alure2-alext.h"

#include "alure2-aliases.h"
#include "alure2-typeviews.h"

#ifndef ALURE_API
 #ifndef ALURE_STATIC_LIB
  #if defined(_WIN32)
   #define ALURE_API __declspec(dllimport)
  #elif defined(__has_attribute)
   #if __has_attribute(visibility)
    #define ALURE_API __attribute__((visibility("default")))
   #endif
  #elif defined(__GNUC__)
   #define ALURE_API __attribute__((visibility("default")))
  #endif
 #endif
 #ifndef ALURE_API
  #define ALURE_API
 #endif
#endif /* ALURE_API */
#ifndef ALURE_TEMPLATE
 #ifndef ALURE_STATIC_LIB
  #define ALURE_TEMPLATE extern template
 #else
  #define ALURE_TEMPLATE template
 #endif
#endif /* ALURE_TEMPLATE */

#ifndef EFXEAXREVERBPROPERTIES_DEFINED
#define EFXEAXREVERBPROPERTIES_DEFINED
typedef struct {
    float flDensity;
    float flDiffusion;
    float flGain;
    float flGainHF;
    float flGainLF;
    float flDecayTime;
    float flDecayHFRatio;
    float flDecayLFRatio;
    float flReflectionsGain;
    float flReflectionsDelay;
    float flReflectionsPan[3];
    float flLateReverbGain;
    float flLateReverbDelay;
    float flLateReverbPan[3];
    float flEchoTime;
    float flEchoDepth;
    float flModulationTime;
    float flModulationDepth;
    float flAirAbsorptionGainHF;
    float flHFReference;
    float flLFReference;
    float flRoomRolloffFactor;
    int   iDecayHFLimit;
} EFXEAXREVERBPROPERTIES, *LPEFXEAXREVERBPROPERTIES;
#endif

#ifndef EFXCHORUSPROPERTIES_DEFINED
#define EFXCHORUSPROPERTIES_DEFINED
typedef struct {
    int iWaveform;
    int iPhase;
    float flRate;
    float flDepth;
    float flFeedback;
    float flDelay;
} EFXCHORUSPROPERTIES, *LPEFXCHORUSPROPERTIES;
#endif

namespace alure {

// Available class interfaces.
class DeviceManager;
class Device;
class Context;
class Listener;
class Buffer;
class Source;
class SourceGroup;
class AuxiliaryEffectSlot;
class Effect;
class Decoder;
class DecoderFactory;
class FileIOFactory;
class MessageHandler;

// Opaque class implementations.
class DeviceManagerImpl;
class DeviceImpl;
class ContextImpl;
class ListenerImpl;
class BufferImpl;
class SourceImpl;
class SourceGroupImpl;
class AuxiliaryEffectSlotImpl;
class EffectImpl;

}

#ifndef ALURE_STATIC_LIB
/****** Explicitly instantiate templates used by the lib ******/
ALURE_TEMPLATE class ALURE_API ALURE_SHARED_PTR_TYPE<alure::DeviceManagerImpl>;
/******/
#endif

namespace alure {

/** Convert a value from decibels to linear gain. */
inline float dBToLinear(float value) { return std::pow(10.0f, value / 20.0f); }
inline double dBToLinear(double value) { return std::pow(10.0, value / 20.0); }
inline double dBToLinear(int value) { return dBToLinear(double(value)); }

/** Convert a value from linear gain to decibels. */
inline float LinearTodB(float value) { return std::log10(value) * 20.0f; }
inline double LinearTodB(double value) { return std::log10(value) * 20.0; }

/**
 * An attribute pair, for passing attributes to Device::createContext and
 * Device::reset.
 */
struct AttributePair {
    ALCint mAttribute;
    ALCint mValue;
};
static_assert(sizeof(AttributePair) == sizeof(ALCint[2]), "Bad AttributePair size");
inline AttributePair AttributesEnd() noexcept { return AttributePair{0, 0}; }


struct FilterParams {
    ALfloat mGain;
    ALfloat mGainHF; // For low-pass and band-pass filters
    ALfloat mGainLF; // For high-pass and band-pass filters
};


class Vector3 {
    Array<ALfloat,3> mValue;

public:
    constexpr Vector3() noexcept
      : mValue{{0.0f, 0.0f, 0.0f}}
    { }
    constexpr Vector3(const Vector3 &rhs) noexcept
      : mValue{{rhs.mValue[0], rhs.mValue[1], rhs.mValue[2]}}
    { }
    constexpr Vector3(ALfloat val) noexcept
      : mValue{{val, val, val}}
    { }
    constexpr Vector3(ALfloat x, ALfloat y, ALfloat z) noexcept
      : mValue{{x, y, z}}
    { }
    Vector3(const ALfloat *vec) noexcept
      : mValue{{vec[0], vec[1], vec[2]}}
    { }

    const ALfloat *getPtr() const noexcept
    { return mValue.data(); }

    ALfloat& operator[](size_t i) noexcept
    { return mValue[i]; }
    constexpr const ALfloat& operator[](size_t i) const noexcept
    { return mValue[i]; }

#define ALURE_DECL_OP(op)                                            \
    constexpr Vector3 operator op(const Vector3 &rhs) const noexcept \
    {                                                                \
        return Vector3(mValue[0] op rhs.mValue[0],                   \
                       mValue[1] op rhs.mValue[1],                   \
                       mValue[2] op rhs.mValue[2]);                  \
    }
    ALURE_DECL_OP(+)
    ALURE_DECL_OP(-)
    ALURE_DECL_OP(*)
    ALURE_DECL_OP(/)
#undef ALURE_DECL_OP
#define ALURE_DECL_OP(op)                             \
    Vector3& operator op(const Vector3 &rhs) noexcept \
    {                                                 \
        mValue[0] op rhs.mValue[0];                   \
        mValue[1] op rhs.mValue[1];                   \
        mValue[2] op rhs.mValue[2];                   \
        return *this;                                 \
    }
    ALURE_DECL_OP(+=)
    ALURE_DECL_OP(-=)
    ALURE_DECL_OP(*=)
    ALURE_DECL_OP(/=)

#undef ALURE_DECL_OP
#define ALURE_DECL_OP(op)                                       \
    constexpr Vector3 operator op(ALfloat scale) const noexcept \
    {                                                           \
        return Vector3(mValue[0] op scale,                      \
                       mValue[1] op scale,                      \
                       mValue[2] op scale);                     \
    }
    ALURE_DECL_OP(*)
    ALURE_DECL_OP(/)
#undef ALURE_DECL_OP
#define ALURE_DECL_OP(op)                        \
    Vector3& operator op(ALfloat scale) noexcept \
    {                                            \
        mValue[0] op scale;                      \
        mValue[1] op scale;                      \
        mValue[2] op scale;                      \
        return *this;                            \
    }
    ALURE_DECL_OP(*=)
    ALURE_DECL_OP(/=)
#undef ALURE_DECL_OP

    constexpr ALfloat getLengthSquared() const noexcept
    { return mValue[0]*mValue[0] + mValue[1]*mValue[1] + mValue[2]*mValue[2]; }
    ALfloat getLength() const noexcept
    { return std::sqrt(getLengthSquared()); }

    constexpr ALfloat getDistanceSquared(const Vector3 &pos) const noexcept
    { return (pos - *this).getLengthSquared(); }
    ALfloat getDistance(const Vector3 &pos) const noexcept
    { return (pos - *this).getLength(); }
};
static_assert(sizeof(Vector3) == sizeof(ALfloat[3]), "Bad Vector3 size");


enum class SampleType {
    UInt8,
    Int16,
    Float32,
    Mulaw
};
ALURE_API const char *GetSampleTypeName(SampleType type);

enum class ChannelConfig {
    /** 1-channel mono sound. */
    Mono,
    /** 2-channel stereo sound. */
    Stereo,
    /** 2-channel rear sound (back-left and back-right). */
    Rear,
    /** 4-channel surround sound. */
    Quad,
    /** 5.1 surround sound. */
    X51,
    /** 6.1 surround sound. */
    X61,
    /** 7.1 surround sound. */
    X71,
    /** 3-channel B-Format, using FuMa channel ordering and scaling. */
    BFormat2D,
    /** 4-channel B-Format, using FuMa channel ordering and scaling. */
    BFormat3D
};
ALURE_API const char *GetChannelConfigName(ChannelConfig cfg);

ALURE_API ALuint FramesToBytes(ALuint frames, ChannelConfig chans, SampleType type);
ALURE_API ALuint BytesToFrames(ALuint bytes, ChannelConfig chans, SampleType type) noexcept;


/** Class for storing a major.minor version number. */
class Version {
    ALuint mMajor : 16;
    ALuint mMinor : 16;

public:
    constexpr Version() noexcept : mMajor(0), mMinor(0) { }
    constexpr Version(ALuint _maj, ALuint _min) noexcept : mMajor(_maj), mMinor(_min) { }
    constexpr Version(const Version&) noexcept = default;

    constexpr ALuint getMajor() const noexcept { return mMajor; }
    constexpr ALuint getMinor() const noexcept { return mMinor; }

    constexpr bool operator==(const Version &rhs) const noexcept
    { return mMajor == rhs.mMajor && mMinor == rhs.mMinor; }
    constexpr bool operator!=(const Version &rhs) const noexcept
    { return !(*this == rhs); }
    constexpr bool operator<=(const Version &rhs) const noexcept
    { return mMajor < rhs.mMajor || (mMajor == rhs.mMajor && mMinor <= rhs.mMinor); }
    constexpr bool operator>=(const Version &rhs) const noexcept
    { return mMajor > rhs.mMajor || (mMajor == rhs.mMajor && mMinor >= rhs.mMinor); }
    constexpr bool operator<(const Version &rhs) const noexcept
    { return mMajor < rhs.mMajor || (mMajor == rhs.mMajor && mMinor < rhs.mMinor); }
    constexpr bool operator>(const Version &rhs) const noexcept
    { return mMajor > rhs.mMajor || (mMajor == rhs.mMajor && mMinor > rhs.mMinor); }

    constexpr bool isZero() const noexcept { return *this == Version{0,0}; }
};


// Tag type to disctate which types are allowed in AutoObj.
template<typename T> struct IsAutoable : std::false_type { };
template<> struct IsAutoable<Device> : std::true_type { };
template<> struct IsAutoable<Context> : std::true_type { };
template<> struct IsAutoable<Source> : std::true_type { };
template<> struct IsAutoable<SourceGroup> : std::true_type { };
template<> struct IsAutoable<AuxiliaryEffectSlot> : std::true_type { };
template<> struct IsAutoable<Effect> : std::true_type { };

/**
 * A local storage container to manage objects in a non-copyable, movable, and
 * auto-destructed manner. Any contained object will have its destroy() method
 * invoked prior to being overwritten or when going out of scope. The purpose
 * of this is to optionally provide RAII semantics to Alure's resources, such
 * as contexts, sources, and effects.
 *
 * Be aware that destruction order is important, as contexts ultimately "own"
 * the resources created from them. Said resources automatically become invalid
 * when their owning context is destroyed. Any AutoObjs containing sources,
 * effects, etc, should already be destroyed or cleared prior to the context
 * being destroyed.
 *
 * Also, it is possible for resource destruction to fail if the destroy()
 * method is called incorrectly (e.g. destroying a source when a different
 * context is current). This normally results in an exception, but because
 * destructors aren't allowed to let exceptions leave the function body,
 * std::terminate will be called as a fatal error instead.
 */
template<typename T>
class AutoObj {
    static_assert(IsAutoable<T>::value, "Invalid type for AutoObj");

    T mObj;

    template<typename U=T>
    EnableIfT<!std::is_same<U,Device>::value,AutoObj&> do_reset(const U &obj={})
    {
        if(mObj) mObj.destroy();
        mObj = obj;
        return *this;
    }
    template<typename U=T>
    EnableIfT<std::is_same<U,Device>::value,AutoObj&> do_reset(const U &obj={})
    {
        if(mObj) mObj.close();
        mObj = obj;
        return *this;
    }

public:
    using element_type = T;

    AutoObj() noexcept = default;
    AutoObj(const AutoObj&) = delete;
    AutoObj(AutoObj &&rhs) noexcept : mObj(rhs.mObj) { rhs.mObj = nullptr; }
    AutoObj(std::nullptr_t) noexcept : mObj(nullptr) { }
    explicit AutoObj(const element_type &rhs) noexcept : mObj(rhs) { }
    ~AutoObj() { do_reset(); }

    AutoObj& operator=(const AutoObj&) = delete;
    AutoObj& operator=(AutoObj &&rhs)
    {
        do_reset(rhs.mObj);
        rhs.mObj = nullptr;
        return *this;
    }

    AutoObj& reset(const element_type &obj) { return do_reset(obj); }

    element_type release() noexcept
    {
        element_type ret = mObj;
        mObj = nullptr;
        return ret;
    }

    element_type& get() noexcept { return mObj; }

    element_type& operator*() noexcept { return mObj; }
    element_type* operator->() noexcept { return &mObj; }

    operator bool() const noexcept { return static_cast<bool>(mObj); }
};

/** Creates an AutoObj for the given input object type. */
template<typename T>
inline AutoObj<T> MakeAuto(const T &obj) { return AutoObj<T>(obj); }


enum class DeviceEnumeration {
    Basic = ALC_DEVICE_SPECIFIER,
    Full = ALC_ALL_DEVICES_SPECIFIER,
    Capture = ALC_CAPTURE_DEVICE_SPECIFIER
};

enum class DefaultDeviceType {
    Basic = ALC_DEFAULT_DEVICE_SPECIFIER,
    Full = ALC_DEFAULT_ALL_DEVICES_SPECIFIER,
    Capture = ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER
};

/**
 * A class managing Device objects and other related functionality. This class
 * is a singleton, only one instance will exist in a process at a time.
 */
class ALURE_API DeviceManager {
    SharedPtr<DeviceManagerImpl> pImpl;

    DeviceManager(SharedPtr<DeviceManagerImpl>&& impl) noexcept;

public:
    /**
     * Retrieves a reference-counted DeviceManager instance. When the last
     * reference goes out of scope, the DeviceManager and any remaining managed
     * resources are automatically cleaned up. Multiple calls will return the
     * same instance as long as there is still a pre-existing reference to the
     * instance, or else a new instance will be created.
     */
    static DeviceManager getInstance();

    DeviceManager() noexcept = default;
    DeviceManager(const DeviceManager&) noexcept = default;
    DeviceManager(DeviceManager&& rhs) noexcept = default;
    ~DeviceManager();

    DeviceManager& operator=(const DeviceManager&) noexcept = default;
    DeviceManager& operator=(DeviceManager&&) noexcept = default;
    DeviceManager& operator=(std::nullptr_t) noexcept { pImpl = nullptr; return *this; };

    operator bool() const noexcept { return pImpl != nullptr; }

    /** Queries the existence of a non-device-specific ALC extension. */
    bool queryExtension(const String &name) const;
    bool queryExtension(const char *name) const;

    /** Enumerates available device names of the given type. */
    Vector<String> enumerate(DeviceEnumeration type) const;
    /** Retrieves the default device of the given type. */
    String defaultDeviceName(DefaultDeviceType type) const;

    /**
     * Opens the playback device given by name, or the default if blank. Throws
     * an exception on error.
     */
    Device openPlayback(const String &name={});
    Device openPlayback(const char *name);

    /**
     * Opens the playback device given by name, or the default if blank.
     * Returns an empty Device on error.
     */
    Device openPlayback(const String &name, const std::nothrow_t&) noexcept;
    Device openPlayback(const char *name, const std::nothrow_t&) noexcept;

    /** Opens the default playback device. Returns an empty Device on error. */
    Device openPlayback(const std::nothrow_t&) noexcept;
};


#define MAKE_PIMPL(BaseT, ImplT)                                              \
private:                                                                      \
    ImplT *pImpl;                                                             \
                                                                              \
public:                                                                       \
    using handle_type = ImplT*;                                               \
                                                                              \
    BaseT() noexcept : pImpl(nullptr) { }                                     \
    BaseT(ImplT *impl) noexcept : pImpl(impl) { }                             \
    BaseT(const BaseT&) noexcept = default;                                   \
    BaseT(BaseT&& rhs) noexcept : pImpl(rhs.pImpl) { rhs.pImpl = nullptr; }   \
                                                                              \
    BaseT& operator=(const BaseT&) noexcept = default;                        \
    BaseT& operator=(BaseT&& rhs) noexcept                                    \
    {                                                                         \
        pImpl = rhs.pImpl; rhs.pImpl = nullptr;                               \
        return *this;                                                         \
    }                                                                         \
                                                                              \
    bool operator==(const BaseT &rhs) const noexcept                          \
    { return pImpl == rhs.pImpl; }                                            \
    bool operator!=(const BaseT &rhs) const noexcept                          \
    { return pImpl != rhs.pImpl; }                                            \
    bool operator<=(const BaseT &rhs) const noexcept                          \
    { return pImpl <= rhs.pImpl; }                                            \
    bool operator>=(const BaseT &rhs) const noexcept                          \
    { return pImpl >= rhs.pImpl; }                                            \
    bool operator<(const BaseT &rhs) const noexcept                           \
    { return pImpl < rhs.pImpl; }                                             \
    bool operator>(const BaseT &rhs) const noexcept                           \
    { return pImpl > rhs.pImpl; }                                             \
                                                                              \
    operator bool() const noexcept { return !!pImpl; }                        \
                                                                              \
    handle_type getHandle() const noexcept { return pImpl; }

enum class PlaybackName {
    Basic = ALC_DEVICE_SPECIFIER,
    Full = ALC_ALL_DEVICES_SPECIFIER
};

class ALURE_API Device {
    MAKE_PIMPL(Device, DeviceImpl)

public:
    /** Retrieves the device name as given by type. */
    String getName(PlaybackName type=PlaybackName::Full) const;
    /** Queries the existence of an ALC extension on this device. */
    bool queryExtension(const String &name) const;
    bool queryExtension(const char *name) const;

    /** Retrieves the ALC version supported by this device. */
    Version getALCVersion() const;

    /**
     * Retrieves the EFX version supported by this device. If the ALC_EXT_EFX
     * extension is unsupported, this will be 0.0.
     */
    Version getEFXVersion() const;

    /** Retrieves the device's playback frequency, in hz. */
    ALCuint getFrequency() const;

    /**
     * Retrieves the maximum number of auxiliary source sends. If ALC_EXT_EFX
     * is unsupported, this will be 0.
     */
    ALCuint getMaxAuxiliarySends() const;

    /**
     * Enumerates available HRTF names. The names are sorted as OpenAL gives
     * them, such that the index of a given name is the ID to use with
     * ALC_HRTF_ID_SOFT.
     *
     * If the ALC_SOFT_HRTF extension is unavailable, this will return an empty
     * vector.
     */
    Vector<String> enumerateHRTFNames() const;

    /**
     * Retrieves whether HRTF is enabled on the device or not.
     *
     * If the ALC_SOFT_HRTF extension is unavailable, this will return false
     * although there could still be HRTF applied at a lower hardware level.
     */
    bool isHRTFEnabled() const;

    /**
     * Retrieves the name of the HRTF currently being used by this device.
     *
     * If HRTF is not currently enabled, this will be empty.
     */
    String getCurrentHRTF() const;

    /**
     * Resets the device, using the specified attributes.
     *
     * If the ALC_SOFT_HRTF extension is unavailable, this will be a no-op.
     */
    void reset(ArrayView<AttributePair> attributes);

    /**
     * Creates a new Context on this device, using the specified attributes.
     * Throws an exception if context creation fails.
     */
    Context createContext(ArrayView<AttributePair> attributes={});
    /**
     * Creates a new Context on this device, using the specified attributes.
     * Returns an empty Context if context creation fails.
     */
    Context createContext(ArrayView<AttributePair> attributes, const std::nothrow_t&) noexcept;
    Context createContext(const std::nothrow_t&) noexcept;

    /**
     * Pauses device processing, stopping updates for its contexts. Multiple
     * calls are allowed but it is not reference counted, so the device will
     * resume after one resumeDSP call.
     *
     * Requires the ALC_SOFT_pause_device extension.
     */
    void pauseDSP();

    /**
     * Resumes device processing, restarting updates for its contexts. Multiple
     * calls are allowed and will no-op.
     */
    void resumeDSP();

    /**
     * Retrieves the current clock time for the device. This starts relative to
     * the device being opened, and does not increment while there are no
     * contexts nor while processing is paused. This is currently based on
     * std::chrono::steady_clock, and so may not exactly match the rate that
     * sources play at. In the future it may utilize an OpenAL extension to
     * retrieve the audio device's real clock which may tic at a subtly
     * different rate than the main clock(s).
     */
    std::chrono::nanoseconds getClockTime();

    /**
     * Closes and frees the device. All previously-created contexts must first
     * be destroyed.
     */
    void close();
};


enum class DistanceModel {
    InverseClamped  = AL_INVERSE_DISTANCE_CLAMPED,
    LinearClamped   = AL_LINEAR_DISTANCE_CLAMPED,
    ExponentClamped = AL_EXPONENT_DISTANCE_CLAMPED,
    Inverse  = AL_INVERSE_DISTANCE,
    Linear   = AL_LINEAR_DISTANCE,
    Exponent = AL_EXPONENT_DISTANCE,
    None = AL_NONE,
};

class ALURE_API Context {
    MAKE_PIMPL(Context, ContextImpl)

public:
    /** Makes the specified context current for OpenAL operations. */
    static void MakeCurrent(Context context);
    /** Retrieves the current context used for OpenAL operations. */
    static Context GetCurrent();

    /**
     * Makes the specified context current for OpenAL operations on the calling
     * thread only. Requires the ALC_EXT_thread_local_context extension on both
     * the context's device and the DeviceManager.
     */
    static void MakeThreadCurrent(Context context);
    /** Retrieves the thread-specific context used for OpenAL operations. */
    static Context GetThreadCurrent();

    /**
     * Destroys the context. The context must not be current when this is
     * called.
     */
    void destroy();

    /** Retrieves the Device this context was created from. */
    Device getDevice();

    void startBatch();
    void endBatch();

    /**
     * Retrieves a Listener instance for this context. Each context will only
     * have one listener, which is automatically destroyed with the context.
     */
    Listener getListener();

    /**
     * Sets a MessageHandler instance which will be used to provide certain
     * messages back to the application. Only one handler may be set for a
     * context at a time. The previously set handler will be returned.
     */
    SharedPtr<MessageHandler> setMessageHandler(SharedPtr<MessageHandler> handler);

    /** Gets the currently-set message handler. */
    SharedPtr<MessageHandler> getMessageHandler() const;

    /**
     * Specifies the desired interval that the background thread will be woken
     * up to process tasks, e.g. keeping streaming sources filled. An interval
     * of 0 means the background thread will only be woken up manually with
     * calls to update. The default is 0.
     */
    void setAsyncWakeInterval(std::chrono::milliseconds interval);

    /**
     * Retrieves the current interval used for waking up the background thread.
     */
    std::chrono::milliseconds getAsyncWakeInterval() const;

    // Functions below require the context to be current

    /**
     * Creates a Decoder instance for the given audio file or resource name.
     */
    SharedPtr<Decoder> createDecoder(StringView name);

    /**
     * Queries if the channel configuration and sample type are supported by
     * the context.
     */
    bool isSupported(ChannelConfig channels, SampleType type) const;

    /**
     * Queries the list of resamplers supported by the context. If the
     * AL_SOFT_source_resampler extension is unsupported this will be an empty
     * array, otherwise there will be at least one entry.
     */
    ArrayView<String> getAvailableResamplers();
    /**
     * Queries the context's default resampler index. Be aware, if the
     * AL_SOFT_source_resampler extension is unsupported the resampler list
     * will be empty and this will resturn 0. If you try to access the
     * resampler list with this index without the extension, undefined behavior
     * will occur (accessing an out of bounds array index).
     */
    ALsizei getDefaultResamplerIndex() const;

    /**
     * Creates and caches a Buffer for the given audio file or resource name.
     * Multiple calls with the same name will return the same Buffer object.
     * Cached buffers must be freed using removeBuffer before destroying the
     * context. If the buffer can't be loaded it will throw an exception.
     */
    Buffer getBuffer(StringView name);

    /**
     * Asynchronously prepares a cached Buffer for the given audio file or
     * resource name. Multiple calls with the same name will return multiple
     * SharedFutures for the same Buffer object. Once called, the buffer must
     * be freed using removeBuffer before destroying the context, even if you
     * never get the Buffer from the SharedFuture.
     *
     * The Buffer will be scheduled to load asynchronously, and the caller gets
     * back a SharedFuture that can be checked later (or waited on) to get the
     * actual Buffer when it's ready. The application must take care to handle
     * exceptions from the SharedFuture in case an unrecoverable error ocurred
     * during the load.
     *
     * If the Buffer is already fully loaded and cached, a SharedFuture is
     * returned in a ready state containing it.
     */
    SharedFuture<Buffer> getBufferAsync(StringView name);

    /**
     * Asynchronously prepares cached Buffers for the given audio file or
     * resource names. Duplicate names and buffers already cached are ignored.
     * Cached buffers must be freed using removeBuffer before destroying the
     * context.
     *
     * The Buffer objects will be scheduled for loading asynchronously, and
     * should be retrieved later when needed using getBufferAsync or getBuffer.
     * Buffers that cannot be loaded, for example due to an unsupported format,
     * will be ignored and a later call to getBuffer or getBufferAsync will
     * throw an exception.
     */
    void precacheBuffersAsync(ArrayView<StringView> names);

    /**
     * Creates and caches a Buffer using the given name by reading the given
     * decoder. The name may alias an audio file, but it must not currently
     * exist in the buffer cache.
     */
    Buffer createBufferFrom(StringView name, SharedPtr<Decoder> decoder);

    /**
     * Asynchronously prepares a cached Buffer using the given name by reading
     * the given decoder. The name may alias an audio file, but it must not
     * currently exist in the buffer cache. Once called, the buffer must be
     * freed using removeBuffer before destroying the context, even if you
     * never get the Buffer from the SharedFuture.
     *
     * The Buffer will be scheduled to load asynchronously, and the caller gets
     * back a SharedFuture that can be checked later (or waited on) to get the
     * actual Buffer when it's ready. The application must take care to handle
     * exceptions from the SharedFuture in case an unrecoverable error ocurred
     * during the load. The decoder must not have its read or seek methods
     * called while the buffer is not ready.
     */
    SharedFuture<Buffer> createBufferAsyncFrom(StringView name, SharedPtr<Decoder> decoder);

    /**
     * Looks for a cached buffer using the given name and returns it. If the
     * given name does not exist in the cache, a null buffer is returned.
     */
    Buffer findBuffer(StringView name);

    /**
     * Looks for an asynchronously-loading buffer using the given name and
     * returns a SharedFuture for it. If the given name does not exist in the
     * cache, an invalid SharedFuture is returned (check with a call to
     * \c SharedFuture::valid).
     *
     * If the Buffer is already fully loaded and cached, a SharedFuture is
     * returned in a ready state containing it.
     */
    SharedFuture<Buffer> findBufferAsync(StringView name);

    /**
     * Deletes the cached Buffer object for the given audio file or resource
     * name, invalidating all Buffer objects with this name. If a source is
     * currently playing the buffer, it will be stopped first.
     */
    void removeBuffer(StringView name);
    /**
     * Deletes the given cached buffer, invalidating all other Buffer objects
     * with the same name. Equivalent to calling
     * removeBuffer(buffer.getName()).
     */
    void removeBuffer(Buffer buffer);

    /**
     * Creates a new Source for playing audio. There is no practical limit to
     * the number of sources you may create. You must call Source::destroy when
     * the source is no longer needed.
     */
    Source createSource();

    AuxiliaryEffectSlot createAuxiliaryEffectSlot();

    Effect createEffect();

    SourceGroup createSourceGroup();

    /** Sets the doppler factor to apply to all source doppler calculations. */
    void setDopplerFactor(ALfloat factor);

    /**
     * Sets the speed of sound propagation, in units per second, to calculate
     * the doppler effect along with other distance-related time effects. The
     * default is 343.3 units per second (a realistic speed assuming 1 meter
     * per unit). If this is adjusted for a different unit scale,
     * Listener::setMetersPerUnit should also be adjusted.
     */
    void setSpeedOfSound(ALfloat speed);

    /**
     * Sets the distance model used to attenuate sources given their distance
     * from the listener. The default, InverseClamped, provides a realistic 1/r
     * reduction in volume (that is, every doubling of distance causes the gain
     * to reduce by half).
     *
     * The Clamped distance models restrict the source distance for the purpose
     * of distance attenuation, so a source won't sound closer than its
     * reference distance or farther than its max distance.
     */
    void setDistanceModel(DistanceModel model);

    /** Updates the context and all sources belonging to this context. */
    void update();
};

class ALURE_API Listener {
    MAKE_PIMPL(Listener, ListenerImpl)

public:
    /** Sets the "master" gain for all context output. */
    void setGain(ALfloat gain);

    /**
     * Specifies the listener's 3D position, velocity, and orientation
     * together (see: setPosition, setVelocity, and setOrientation).
     */
    void set3DParameters(const Vector3 &position, const Vector3 &velocity, const std::pair<Vector3,Vector3> &orientation);

    /** Specifies the listener's 3D position. */
    void setPosition(const Vector3 &position);
    void setPosition(const ALfloat *pos);

    /**
     * Specifies the listener's 3D velocity, in units per second. As with
     * OpenAL, this does not actually alter the listener's position, and
     * instead just alters the pitch as determined by the doppler effect.
     */
    void setVelocity(const Vector3 &velocity);
    void setVelocity(const ALfloat *vel);

    /**
     * Specifies the listener's 3D orientation, using position-relative 'at'
     * and 'up' direction vectors.
     */
    void setOrientation(const std::pair<Vector3,Vector3> &orientation);
    void setOrientation(const ALfloat *at, const ALfloat *up);
    void setOrientation(const ALfloat *ori);

    /**
     * Sets the number of meters per unit, used for various effects that rely
     * on the distance in meters including air absorption and initial reverb
     * decay. If this is changed, it's strongly recommended to also set the
     * speed of sound (e.g. context.setSpeedOfSound(343.3 / m_u) to maintain a
     * realistic 343.3m/s for sound propagation).
     */
    void setMetersPerUnit(ALfloat m_u);
};


class ALURE_API Buffer {
    MAKE_PIMPL(Buffer, BufferImpl)

public:
    /** Retrieves the length of the buffer in sample frames. */
    ALuint getLength() const;

    /** Retrieves the buffer's frequency in hz. */
    ALuint getFrequency() const;

    /** Retrieves the buffer's sample configuration. */
    ChannelConfig getChannelConfig() const;

    /** Retrieves the buffer's sample type. */
    SampleType getSampleType() const;

    /**
     * Retrieves the storage size used by the buffer, in bytes. Note that the
     * size in bytes may not be what you expect from the length, as it may take
     * more space internally than the ChannelConfig and SampleType suggest to
     * be more efficient.
     */
    ALuint getSize() const;

    /**
     * Sets the buffer's loop points, used for looping sources. If the current
     * context does not support the AL_SOFT_loop_points extension, start and
     * end must be 0 and getLength() respectively. Otherwise, start must be
     * less than end, and end must be less than or equal to getLength().
     *
     * The buffer must not be in use when this method is called.
     *
     * \param start The starting point, in sample frames (inclusive).
     * \param end The ending point, in sample frames (exclusive).
     */
    void setLoopPoints(ALuint start, ALuint end);

    /** Retrieves the current loop points as a [start,end) pair. */
    std::pair<ALuint,ALuint> getLoopPoints() const;

    /** Retrieves the Source objects currently playing the buffer. */
    Vector<Source> getSources() const;

    /** Retrieves the name the buffer was created with. */
    StringView getName() const;

    /**
     * Queries the number of sources currently using the buffer. Be aware that
     * you need to call \c Context::update to reliably ensure the count is kept
     * updated for when sources reach their end. This is equivalent to calling
     * getSources().size().
     */
    size_t getSourceCount() const;
};


enum class Spatialize {
    Off = AL_FALSE,
    On = AL_TRUE,
    Auto = 0x0002 /* AL_AUTO_SOFT */
};

class ALURE_API Source {
    MAKE_PIMPL(Source, SourceImpl)

public:
    /**
     * Plays the source using a buffer. The same buffer may be played from
     * multiple sources simultaneously.
     */
    void play(Buffer buffer);
    /**
     * Plays the source by asynchronously streaming audio from a decoder. The
     * given decoder must *NOT* have its read or seek methods called from
     * elsewhere while in use.
     *
     * \param decoder The decoder object to play audio from.
     * \param chunk_len The number of sample frames to read for each chunk
     *        update. Smaller values will require more frequent updates and
     *        larger values will handle more data with each chunk.
     * \param queue_size The number of chunks to keep queued during playback.
     *        Smaller values use less memory while larger values improve
     *        protection against underruns.
     */
    void play(SharedPtr<Decoder> decoder, ALsizei chunk_len, ALsizei queue_size);

    /**
     * Prepares to play a source using a future buffer. The method will return
     * right away and the source will begin playing once the future buffer
     * becomes ready. If the future buffer is already ready, it begins playing
     * immediately as if you called play(future_buffer.get()).
     *
     * The future buffer is checked during calls to \c Context::update and the
     * source will start playback once the future buffer reports it's ready.
     * Use the isPending method to check if the source is still waiting for the
     * future buffer.
     */
    void play(SharedFuture<Buffer> future_buffer);

    /**
     * Stops playback, releasing the buffer or decoder reference. Any pending
     * playback from a future buffer is canceled.
     */
    void stop();

    /**
     * Fades the source to the specified gain over the given duration, at which
     * point playback will stop. This gain is in addition to the base gain, and
     * must be greater than 0 and less than 1. The duration must also be
     * greater than 0.
     *
     * The fading is logarithmic. As a result, the initial drop-off may happen
     * faster than expected but the fading is more perceptually consistant over
     * the given duration. It will take just as much time to go from -6dB to
     * -12dB as it will to go from -40dB to -46dB, for example.
     *
     * Pending playback from a future buffer is not immediately canceled, but
     * the fade timer starts with this call. If the future buffer then becomes
     * ready, it will start mid-fade. Pending playback will be canceled if the
     * fade out completes before the future buffer becomes ready.
     *
     * Fading is updated during calls to \c Context::update, which should be
     * called regularly (30 to 50 times per second) for the fading to be
     * smooth.
     */
    void fadeOutToStop(ALfloat gain, std::chrono::milliseconds duration);

    /** Pauses the source if it is playing. */
    void pause();

    /** Resumes the source if it is paused. */
    void resume();

    /** Specifies if the source is waiting to play a future buffer. */
    bool isPending() const;

    /** Specifies if the source is currently playing. */
    bool isPlaying() const;

    /** Specifies if the source is currently paused. */
    bool isPaused() const;

    /**
     * Specifies if the source is currently playing or waiting to play a future
     * buffer.
     */
    bool isPlayingOrPending() const;

    /**
     * Sets this source as a child of the given source group. The given source
     * group's parameters will influence this and all other sources that belong
     * to it. A source can only be the child of one source group at a time,
     * although that source group may belong to another source group.
     *
     * Passing in a null group removes it from its current source group.
     */
    void setGroup(SourceGroup group);

    /** Retrieves the source group this source belongs to. */
    SourceGroup getGroup() const;

    /**
     * Specifies the source's playback priority. The lowest priority sources
     * will be forcefully stopped when no more mixing sources are available and
     * higher priority sources are played.
     */
    void setPriority(ALuint priority);
    /** Retrieves the source's priority. */
    ALuint getPriority() const;

    /**
     * Sets the source's offset, in sample frames. If the source is playing or
     * paused, it will go to that offset immediately, otherwise the source will
     * start at the specified offset the next time it's played.
     */
    void setOffset(uint64_t offset);
    /**
     * Retrieves the source offset in sample frames and its latency in nano-
     * seconds. For streaming sources this will be the offset based on the
     * decoder's read position.
     *
     * If the AL_SOFT_source_latency extension is unsupported, the latency will
     * be 0.
     */
    std::pair<uint64_t,std::chrono::nanoseconds> getSampleOffsetLatency() const;
    uint64_t getSampleOffset() const { return std::get<0>(getSampleOffsetLatency()); }
    /**
     * Retrieves the source offset and latency in seconds. For streaming
     * sources this will be the offset based on the decoder's read position.
     *
     * If the AL_SOFT_source_latency extension is unsupported, the latency will
     * be 0.
     */
    std::pair<Seconds,Seconds> getSecOffsetLatency() const;
    Seconds getSecOffset() const { return std::get<0>(getSecOffsetLatency()); }

    /**
     * Specifies if the source should loop on the Buffer or Decoder object's
     * loop points.
     */
    void setLooping(bool looping);
    bool getLooping() const;

    /**
     * Specifies a linear pitch shift base. A value of 1.0 is the default
     * normal speed.
     */
    void setPitch(ALfloat pitch);
    ALfloat getPitch() const;

    /**
     * Specifies the base linear gain. A value of 1.0 is the default normal
     * volume.
     */
    void setGain(ALfloat gain);
    ALfloat getGain() const;

    /**
     * Specifies the minimum and maximum gain. The source's gain is clamped to
     * this range after distance attenuation and cone attenuation are applied
     * to the gain base, although before the filter gain adjustements.
     */
    void setGainRange(ALfloat mingain, ALfloat maxgain);
    std::pair<ALfloat,ALfloat> getGainRange() const;
    ALfloat getMinGain() const { return std::get<0>(getGainRange()); }
    ALfloat getMaxGain() const { return std::get<1>(getGainRange()); }

    /**
     * Specifies the reference distance and maximum distance the source will
     * use for the current distance model. For Clamped distance models, the
     * source's calculated distance is clamped to the specified range before
     * applying distance-related attenuation.
     *
     * For all distance models, the reference distance is the distance at which
     * the source's volume will not have any extra attenuation (an effective
     * gain multiplier of 1).
     */
    void setDistanceRange(ALfloat refdist, ALfloat maxdist);
    std::pair<ALfloat,ALfloat> getDistanceRange() const;
    ALfloat getReferenceDistance() const { return std::get<0>(getDistanceRange()); }
    ALfloat getMaxDistance() const { return std::get<1>(getDistanceRange()); }

    /**
     * Specifies the source's 3D position, velocity, and direction together
     * (see: setPosition, setVelocity, and setDirection).
     */
    void set3DParameters(const Vector3 &position, const Vector3 &velocity, const Vector3 &direction);

    /**
     * Specifies the source's 3D position, velocity, and orientation together
     * (see: setPosition, setVelocity, and setOrientation).
     */
    void set3DParameters(const Vector3 &position, const Vector3 &velocity, const std::pair<Vector3,Vector3> &orientation);

    /** Specifies the source's 3D position. */
    void setPosition(const Vector3 &position);
    void setPosition(const ALfloat *pos);
    Vector3 getPosition() const;

    /**
     * Specifies the source's 3D velocity, in units per second. As with OpenAL,
     * this does not actually alter the source's position, and instead just
     * alters the pitch as determined by the doppler effect.
     */
    void setVelocity(const Vector3 &velocity);
    void setVelocity(const ALfloat *vel);
    Vector3 getVelocity() const;

    /**
     * Specifies the source's 3D facing direction. Deprecated in favor of
     * setOrientation.
     */
    void setDirection(const Vector3 &direction);
    void setDirection(const ALfloat *dir);
    Vector3 getDirection() const;

    /**
     * Specifies the source's 3D orientation, using position-relative 'at' and
     * 'up' direction vectors. Note: unlike the AL_EXT_BFORMAT extension this
     * property comes from, this also affects the facing direction, superceding
     * setDirection.
     */
    void setOrientation(const std::pair<Vector3,Vector3> &orientation);
    void setOrientation(const ALfloat *at, const ALfloat *up);
    void setOrientation(const ALfloat *ori);
    std::pair<Vector3,Vector3> getOrientation() const;

    /**
     * Specifies the source's cone angles, in degrees. The inner angle is the
     * area within which the listener will hear the source with no extra
     * attenuation, while the listener being outside of the outer angle will
     * hear the source attenuated according to the outer cone gains. The area
     * follows the facing direction, so for example an inner angle of 180 means
     * the entire front face of the source is in the inner cone.
     */
    void setConeAngles(ALfloat inner, ALfloat outer);
    std::pair<ALfloat,ALfloat> getConeAngles() const;
    ALfloat getInnerConeAngle() const { return std::get<0>(getConeAngles()); }
    ALfloat getOuterConeAngle() const { return std::get<1>(getConeAngles()); }

    /**
     * Specifies the linear gain multiplier when the listener is outside of the
     * source's outer cone area. The specified gain applies to all frequencies,
     * while gainhf applies extra attenuation to high frequencies creating a
     * low-pass effect.
     *
     * \param gainhf has no effect without the ALC_EXT_EFX extension.
     */
    void setOuterConeGains(ALfloat gain, ALfloat gainhf=1.0f);
    std::pair<ALfloat,ALfloat> getOuterConeGains() const;
    ALfloat getOuterConeGain() const { return std::get<0>(getOuterConeGains()); }
    ALfloat getOuterConeGainHF() const { return std::get<1>(getOuterConeGains()); }

    /**
     * Specifies the rolloff factors for the direct and send paths. This is
     * effectively a distance scaling relative to the reference distance. Note:
     * the room rolloff factor is 0 by default, disabling distance attenuation
     * for send paths. This is because the reverb engine will, by default,
     * apply a more realistic room decay based on the reverb decay time and
     * distance.
     */
    void setRolloffFactors(ALfloat factor, ALfloat roomfactor=0.0f);
    std::pair<ALfloat,ALfloat> getRolloffFactors() const;
    ALfloat getRolloffFactor() const { return std::get<0>(getRolloffFactors()); }
    ALfloat getRoomRolloffFactor() const { return std::get<1>(getRolloffFactors()); }

    /**
     * Specifies the doppler factor for the doppler effect's pitch shift. This
     * effectively scales the source and listener velocities for the doppler
     * calculation.
     */
    void setDopplerFactor(ALfloat factor);
    ALfloat getDopplerFactor() const;

    /**
     * Specifies if the source's position, velocity, and direction/orientation
     * are relative to the listener.
     */
    void setRelative(bool relative);
    bool getRelative() const;

    /**
     * Specifies the source's radius. This causes the source to behave as if
     * every point within the spherical area emits sound.
     *
     * Has no effect without the AL_EXT_SOURCE_RADIUS extension.
     */
    void setRadius(ALfloat radius);
    ALfloat getRadius() const;

    /**
     * Specifies the left and right channel angles, in radians, when playing a
     * stereo buffer or stream. The angles go counter-clockwise, with 0 being
     * in front and positive values going left.
     *
     * Has no effect without the AL_EXT_STEREO_ANGLES extension.
     */
    void setStereoAngles(ALfloat leftAngle, ALfloat rightAngle);
    std::pair<ALfloat,ALfloat> getStereoAngles() const;

    /**
     * Specifies if the source always has 3D spatialization features (On),
     * never has 3D spatialization features (Off), or if spatialization is
     * enabled based on playing a mono sound or not (Auto, default).
     *
     * Has no effect without the AL_SOFT_source_spatialize extension.
     */
    void set3DSpatialize(Spatialize spatialize);
    Spatialize get3DSpatialize() const;

    /**
     * Specifies the index of the resampler to use for this source. The index
     * is from the resamplers returned by \c Context::getAvailableResamplers,
     * and must be 0 or greater.
     *
     * Has no effect without the AL_SOFT_source_resampler extension.
     */
    void setResamplerIndex(ALsizei index);
    ALsizei getResamplerIndex() const;

    /**
     * Specifies a multiplier for the amount of atmospheric high-frequency
     * absorption, ranging from 0 to 10. A factor of 1 results in a nominal
     * -0.05dB per meter, with higher values simulating foggy air and lower
     * values simulating dryer air. The default is 0.
     */
    void setAirAbsorptionFactor(ALfloat factor);
    ALfloat getAirAbsorptionFactor() const;

    /**
     * Specifies to automatically apply adjustments to the direct path's high-
     * frequency gain, and the send paths' gain and high-frequency gain. The
     * default is true for all.
     */
    void setGainAuto(bool directhf, bool send, bool sendhf);
    std::tuple<bool,bool,bool> getGainAuto() const;
    bool getDirectGainHFAuto() const { return std::get<0>(getGainAuto()); }
    bool getSendGainAuto() const { return std::get<1>(getGainAuto()); }
    bool getSendGainHFAuto() const { return std::get<2>(getGainAuto()); }

    /** Sets the filter properties on the direct path signal. */
    void setDirectFilter(const FilterParams &filter);
    /**
     * Sets the filter properties on the given send path signal. Any auxiliary
     * effect slot on the send path remains in place.
     */
    void setSendFilter(ALuint send, const FilterParams &filter);
    /**
     * Connects the effect slot to the given send path. Any filter properties
     * on the send path remain as they were.
     */
    void setAuxiliarySend(AuxiliaryEffectSlot slot, ALuint send);
    /**
     * Connects the effect slot to the given send path, using the filter
     * properties.
     */
    void setAuxiliarySendFilter(AuxiliaryEffectSlot slot, ALuint send, const FilterParams &filter);

    /** Destroys the source, stopping playback and releasing resources. */
    void destroy();
};


class ALURE_API SourceGroup {
    MAKE_PIMPL(SourceGroup, SourceGroupImpl)

public:
    /**
     * Adds this source group as a subgroup of the specified source group. This
     * method will throw an exception if this group is being added to a group
     * it has as a sub-group (i.e. it would create a circular sub-group chain).
     */
    void setParentGroup(SourceGroup group);

    /** Retrieves the source group this source group is a child of. */
    SourceGroup getParentGroup() const;

    /** Returns the list of sources currently in the group. */
    Vector<Source> getSources() const;

    /** Returns the list of subgroups currently in the group. */
    Vector<SourceGroup> getSubGroups() const;

    /**
     * Sets the source group gain, which accumulates with its sources' and
     * sub-groups' gain.
     */
    void setGain(ALfloat gain);
    /** Gets the source group gain. */
    ALfloat getGain() const;

    /**
     * Sets the source group pitch, which accumulates with its sources' and
     * sub-groups' pitch.
     */
    void setPitch(ALfloat pitch);
    /** Gets the source group pitch. */
    ALfloat getPitch() const;

    /**
     * Pauses all currently-playing sources that are under this group,
     * including sub-groups.
     */
    void pauseAll() const;
    /**
     * Resumes all paused sources that are under this group, including
     * sub-groups.
     */
    void resumeAll() const;

    /** Stops all sources that are under this group, including sub-groups. */
    void stopAll() const;

    /**
     * Destroys the source group, removing all sources from it before being
     * freed.
     */
    void destroy();
};


struct SourceSend {
    Source mSource;
    ALuint mSend;
};

class ALURE_API AuxiliaryEffectSlot {
    MAKE_PIMPL(AuxiliaryEffectSlot, AuxiliaryEffectSlotImpl)

public:
    void setGain(ALfloat gain);
    /**
     * If set to true, the reverb effect will automatically apply adjustments
     * to the source's send slot gains based on the effect properties.
     *
     * Has no effect when using non-reverb effects. Default is true.
     */
    void setSendAuto(bool sendauto);

    /**
     * Updates the effect slot with a new effect. The given effect object may
     * be altered or destroyed without affecting the effect slot.
     */
    void applyEffect(Effect effect);

    /**
     * Destroys the effect slot, returning it to the system. If the effect slot
     * is currently set on a source send, it will be removed first.
     */
    void destroy();

    /**
     * Retrieves each Source object and its pairing send this effect slot is
     * set on.
     */
    Vector<SourceSend> getSourceSends() const;

    /**
     * Queries the number of source sends the effect slot is used by. This is
     * equivalent to calling getSourceSends().size().
     */
    size_t getUseCount() const;
};


class ALURE_API Effect {
    MAKE_PIMPL(Effect, EffectImpl)

public:
    /**
     * Updates the effect with the specified reverb properties. If the
     * EAXReverb effect is not supported, it will automatically attempt to
     * downgrade to the Standard Reverb effect.
     */
    void setReverbProperties(const EFXEAXREVERBPROPERTIES &props);

    /**
     * Updates the effect with the specified chorus properties. If the chorus
     * effect is not supported, an exception will be thrown.
     */
    void setChorusProperties(const EFXCHORUSPROPERTIES &props);

    void destroy();
};


/**
 * Audio decoder interface. Applications may derive from this, implementing the
 * necessary methods, and use it in places the API wants a Decoder object.
 */
class ALURE_API Decoder {
public:
    virtual ~Decoder();

    /** Retrieves the sample frequency, in hz, of the audio being decoded. */
    virtual ALuint getFrequency() const noexcept = 0;
    /** Retrieves the channel configuration of the audio being decoded. */
    virtual ChannelConfig getChannelConfig() const noexcept = 0;
    /** Retrieves the sample type of the audio being decoded. */
    virtual SampleType getSampleType() const noexcept = 0;

    /**
     * Retrieves the total length of the audio, in sample frames. If unknown,
     * returns 0. Note that if the returned length is 0, the decoder may not be
     * used to load a Buffer.
     */
    virtual uint64_t getLength() const noexcept = 0;
    /**
     * Seek to pos, specified in sample frames. Returns true if the seek was
     * successful.
     */
    virtual bool seek(uint64_t pos) noexcept = 0;

     /**
     * Retrieves if the audio file has any loop point embedded.
     */
    virtual bool hasLoopPoints() const noexcept = 0;

    /**
     * Retrieves the loop points, in sample frames, as a [start,end) pair. If
     * start >= end, all available samples are included in the loop.
     */
    virtual std::pair<uint64_t, uint64_t> getLoopPoints() const noexcept = 0;

    /**
     * Decodes count sample frames, writing them to ptr, and returns the number
     * of sample frames written. Returning less than the requested count
     * indicates the end of the audio.
     */
    virtual ALuint read(ALvoid *ptr, ALuint count) noexcept = 0;
};

/**
 * Audio decoder factory interface. Applications may derive from this,
 * implementing the necessary methods, and use it in places the API wants a
 * DecoderFactory object.
 */
class ALURE_API DecoderFactory {
public:
    virtual ~DecoderFactory();

    /**
     * Creates and returns a Decoder instance for the given resource file. If
     * the decoder needs to retain the file handle for reading as-needed, it
     * should move the UniquePtr to internal storage.
     *
     * \return nullptr if a decoder can't be created from the file.
     */
    virtual SharedPtr<Decoder> createDecoder(UniquePtr<std::istream> &file) noexcept = 0;
};

/**
 * Registers a decoder factory for decoding audio. Registered factories are
 * used in lexicographical order, e.g. if Factory1 is registered with name1 and
 * Factory2 is registered with name2, Factory1 will be used before Factory2 if
 * name1 < name2. Internal decoder factories are always used after registered
 * ones.
 *
 * Alure retains a reference to the DecoderFactory instance and will release it
 * (destructing the object) when the library unloads.
 *
 * \param name A unique name identifying this decoder factory.
 * \param factory A DecoderFactory instance used to create Decoder instances.
 */
ALURE_API void RegisterDecoder(StringView name, UniquePtr<DecoderFactory> factory);

/**
 * Unregisters a decoder factory by name. Alure returns the instance back to
 * the application.
 *
 * \param name The unique name identifying a previously-registered decoder
 * factory.
 *
 * \return The unregistered decoder factory instance, or 0 (nullptr) if a
 * decoder factory with the given name doesn't exist.
 */
ALURE_API UniquePtr<DecoderFactory> UnregisterDecoder(StringView name) noexcept;


/**
 * A file I/O factory interface. Applications may derive from this and set an
 * instance to be used by the audio decoders. By default, the library uses
 * standard I/O.
 */
class ALURE_API FileIOFactory {
public:
    /**
     * Sets the factory instance to be used by the audio decoders. If a
     * previous factory was set, it's returned to the application. Passing in a
     * nullptr reverts to the default.
     */
    static UniquePtr<FileIOFactory> set(UniquePtr<FileIOFactory> factory) noexcept;

    /**
     * Gets the current FileIOFactory instance being used by the audio
     * decoders.
     */
    static FileIOFactory &get() noexcept;

    virtual ~FileIOFactory();

    /** Opens a read-only binary file for the given name. */
    virtual UniquePtr<std::istream> openFile(const String &name) noexcept = 0;
};


/**
 * A message handler interface. Applications may derive from this and set an
 * instance on a context to receive messages. The base methods are no-ops, so
 * derived classes only need to implement methods for relevant messages.
 *
 * It's recommended that applications mark their handler methods using the
 * override keyword, to ensure they're properly overriding the base methods in
 * case they change.
 */
class ALURE_API MessageHandler {
public:
    virtual ~MessageHandler();

    /**
     * Called when the given device has been disconnected and is no longer
     * usable for output. As per the ALC_EXT_disconnect specification,
     * disconnected devices remain valid, however all playing sources are
     * automatically stopped, any sources that are attempted to play will
     * immediately stop, and new contexts may not be created on the device.
     *
     * Note that connection status is checked during Context::update calls, so
     * that method must be called regularly to be notified when a device is
     * disconnected. This method may not be called if the device lacks support
     * for the ALC_EXT_disconnect extension.
     */
    virtual void deviceDisconnected(Device device) noexcept;

    /**
     * Called when the given source reaches the end of the buffer or stream.
     *
     * Sources that stopped automatically will be detected upon a call to
     * Context::update.
     */
    virtual void sourceStopped(Source source) noexcept;

    /**
     * Called when the given source was forced to stop. This can be because
     * either there were no more mixing sources and a higher-priority source
     * preempted it, it's part of a SourceGroup (or sub-group thereof) that had
     * its SourceGroup::stopAll method called, or it was playing a buffer
     * that's getting removed.
     */
    virtual void sourceForceStopped(Source source) noexcept;

    /**
     * Called when a new buffer is about to be created and loaded. May be
     * called asynchronously for buffers being loaded asynchronously.
     *
     * \param name The resource name, as passed to Context::getBuffer.
     * \param channels Channel configuration of the given audio data.
     * \param type Sample type of the given audio data.
     * \param samplerate Sample rate of the given audio data.
     * \param data The audio data that is about to be fed to the OpenAL buffer.
     */
    virtual void bufferLoading(StringView name, ChannelConfig channels, SampleType type, ALuint samplerate, ArrayView<ALbyte> data) noexcept;

    /**
     * Called when a resource isn't found, allowing the app to substitute in a
     * different resource. For buffers being cached, the original name will
     * still be used for the cache entry so the app doesn't have to keep track
     * of substituted resource names.
     *
     * This will be called again if the new name also isn't found.
     *
     * \param name The resource name that was not found.
     * \return The replacement resource name to use instead. Returning an empty
     *         string means to stop trying.
     */
    virtual String resourceNotFound(StringView name) noexcept;
};

#undef MAKE_PIMPL

} // namespace alure

#endif /* AL_ALURE2_H */
