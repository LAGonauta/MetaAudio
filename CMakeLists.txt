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
    src/snd_local.cpp
    src/SoundSources/SoundSourceFactory.cpp
    src/SoundSources/StaticSoundSource.cpp
    src/SoundSources/StreamingSoundSource.cpp)

add_library(MetaAudio SHARED ${SOURCES})
target_compile_definitions(MetaAudio PRIVATE AUDIO_EXPORTS)
add_compile_definitions(NOMINMAX PICOJSON_USE_INT64)

target_include_directories(MetaAudio
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/externals/picojson
        ${PROJECT_SOURCE_DIR}/externals/steamaudio/include
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

add_subdirectory(externals/openal-soft)

add_subdirectory(externals/alure)

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
    target_compile_options(alure2 PUBLIC "/Zc:__cplusplus")
endif()

set_property(TARGET alure2 PROPERTY CXX_STANDARD 17)

add_subdirectory(externals/metahook)

add_dependencies(MetaAudio alure2)

target_link_libraries(MetaAudio PRIVATE OpenAL alure2)
