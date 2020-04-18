cmake_minimum_required(VERSION 3.9)

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(CMAKE_CXX_STANDARD 17)

project(MetaAudio)

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
    src/snd_local.cpp)

add_library(MetaAudio SHARED ${SOURCES})
target_compile_definitions(MetaAudio PRIVATE AUDIO_EXPORTS)
add_compile_definitions(NOMINMAX PICOJSON_USE_INT64)

target_include_directories(MetaAudio
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/externals/picojson
        ${PROJECT_SOURCE_DIR}/externals/alure/include/AL
        ${PROJECT_SOURCE_DIR}/externals/openal-soft/include/AL
        ${PROJECT_SOURCE_DIR}/externals/metahook/include
        ${PROJECT_SOURCE_DIR}/externals/metahook/include/interface
        ${PROJECT_SOURCE_DIR}/externals/metahook/HLSDK/common
        ${PROJECT_SOURCE_DIR}/externals/metahook/HLSDK/cl_dll
        ${PROJECT_SOURCE_DIR}/externals/metahook/HLSDK/engine
        ${PROJECT_SOURCE_DIR}/externals/metahook/HLSDK/public
        ${PROJECT_SOURCE_DIR}/externals/metahook/HLSDK/pm_shared
)

if (NOT CMAKE_VERSION VERSION_LESS "3.13")
    # CMP0077: option() honors normal variables
    # https://cmake.org/cmake/help/latest/policy/CMP0077.html
    cmake_policy(SET CMP0077 NEW)
endif()

OPTION(ALSOFT_EXAMPLES OFF)
OPTION(ALSOFT_TESTS OFF)
OPTION(ALSOFT_UTILS OFF)
add_subdirectory(externals/openal-soft)

OPTION(ALURE_INSTALL OFF)
OPTION(ALURE_BUILD_STATIC OFF)
OPTION(ALURE_ENABLE_WAVE OFF)
OPTION(ALURE_ENABLE_VORBIS OFF)
OPTION(ALURE_ENABLE_FLAC OFF)
SET(ALURE_ENABLE_SNDFILE ON)
SET(SNDFILE_LIBRARY "C:\\Program Files (x86)\\Mega-Nerd\\libsndfile\\lib\\libsndfile-1.lib")
SET(SNDFILE_INCLUDE_DIR "C:\\Program Files (x86)\\Mega-Nerd\\libsndfile\\include")
OPTION(ALURE_BUILD_EXAMPLES OFF)
add_subdirectory(externals/alure)

add_subdirectory(externals/metahook)

add_subdirectory(externals/steamaudio)

add_dependencies(MetaAudio SteamAudio)

target_link_libraries(MetaAudio PRIVATE OpenAL alure2 SteamAudio)
