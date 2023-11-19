cmake_minimum_required(VERSION 3.9)

project(MetaAudio)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Configure Alure2 the way we need it
option(METAAUDIO_STATIC_ALURE "Link Alure2 statically" ON)
if(METAAUDIO_STATIC_ALURE)
    set(ALURE2_LIBRARY alure2_s)
    option(ALURE_BUILD_SHARED "Shared Alure disabled" OFF)
else()
    set(ALURE2_LIBRARY alure2)
    option(ALURE_BUILD_STATIC "Static Alure disabled" OFF)
endif()
option(ALURE_ENABLE_MINIMP3 "" OFF)
option(ALURE_ENABLE_WAVE "" OFF)
option(ALURE_ENABLE_OPUS "" OFF)
option(ALURE_ENABLE_VORBIS "" OFF)
option(ALURE_ENABLE_FLAC "" OFF)
option(ALURE_INSTALL "" OFF)
option(ALURE_BUILD_EXAMPLES "" OFF)
# MetaAudio requires libSndFile

set(SOURCES
    src/exportfuncs.cpp
    src/metaaudio.cpp
    src/Effects/EnvEffects.cpp
    src/Loaders/SteamAudioMapMeshLoader.cpp
    src/snd_hook.cpp
    src/Voice/VoiceDecoder.cpp
    src/Vox/VoxManager.cpp
    src/Loaders/LocalAudioDecoder.cpp
    src/Utilities/AudioCache.cpp
    src/interface.cpp
    src/Config/EfxJsonReader.cpp
    src/Effects/GoldSrcOcclusionCalculator.cpp
    src/Effects/SteamAudioOcclusionCalculator.cpp
    src/Utilities/Fade.cpp
    src/Workarounds/NoWorkarounds.cpp
    src/Workarounds/XFiWorkarounds.cpp
    src/Loaders/GoldSrcFileBuf.cpp
    src/Loaders/GoldSrcFileStream.cpp
    src/Loaders/GoldSrcFileFactory.cpp
    src/Loaders/SoundLoader.cpp
    src/AudioEngine.cpp
    src/Utilities/ChannelManager.cpp
    src/snd_local.cpp
    src/SoundSources/SoundSourceFactory.cpp
    src/SoundSources/BaseSoundSource.cpp
    src/SoundSources/StaticSoundSource.cpp
    src/SoundSources/StreamingSoundSource.cpp
)

add_library(MetaAudio SHARED ${SOURCES})

set_property(TARGET MetaAudio PROPERTY CXX_STANDARD 17)

target_compile_definitions(MetaAudio PRIVATE AUDIO_EXPORTS)
add_compile_definitions(NOMINMAX PICOJSON_USE_INT64)

target_include_directories(MetaAudio
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/externals/picojson
        ${PROJECT_SOURCE_DIR}/externals/steamaudio/include
        ${PROJECT_SOURCE_DIR}/externals/alure/include/AL
        ${PROJECT_SOURCE_DIR}/externals/openal-soft/include/AL
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/Interface
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/HLSDK/common
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/HLSDK/cl_dll
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/HLSDK/engine
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/HLSDK/public
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/HLSDK/pm_shared
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/capstone/include/capstone
)

find_package(OpenAL REQUIRED)
add_subdirectory(externals/alure)

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
    # Force C++17 as mpark/variant doesn't compile with MSVC
    set_property(TARGET ${ALURE2_LIBRARY} PROPERTY CXX_STANDARD 17)
    target_compile_options(${ALURE2_LIBRARY} PUBLIC "/Zc:__cplusplus")
    target_compile_options(MetaAudio PUBLIC "/Zc:__cplusplus")
endif()

add_dependencies(MetaAudio ${ALURE2_LIBRARY})

target_link_libraries(MetaAudio PRIVATE ${OPENAL_LIBRARY} ${ALURE2_LIBRARY})
