cmake_minimum_required(VERSION 3.8)
project(MetaAudio)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/exportfuncs.cpp
    src/plugins.cpp
    src/snd_dma.cpp
    src/snd_fx.cpp
    src/snd_hook.cpp
    src/snd_loader.cpp
    src/snd_mem.cpp
    src/snd_voice.cpp
    src/snd_vox.cpp
    src/snd_wav.cpp
    src/zone.cpp
)

add_library(MetaAudio SHARED ${SOURCES})
target_compile_definitions(MetaAudio PRIVATE AUDIO_EXPORTS)

target_include_directories(MetaAudio
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/externals
        ${PROJECT_SOURCE_DIR}/../MetaHook/include
        ${PROJECT_SOURCE_DIR}/../MetaHook/include/interface
        ${PROJECT_SOURCE_DIR}/../MetaHook/HLSDK/common
        ${PROJECT_SOURCE_DIR}/../MetaHook/HLSDK/cl_dll
        ${PROJECT_SOURCE_DIR}/../MetaHook/HLSDK/engine
        ${PROJECT_SOURCE_DIR}/../MetaHook/HLSDK/public
        ${PROJECT_SOURCE_DIR}/../MetaHook/HLSDK/pm_shared
)

add_library(libraries INTERFACE)
target_link_libraries(libraries
    INTERFACE
        ${PROJECT_SOURCE_DIR}/externals/alure/alure2.lib
)


target_link_libraries(MetaAudio libraries)