cmake_minimum_required(VERSION 3.9)

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(CMAKE_CXX_STANDARD 17)

project(MetaAudio)

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
    src/interface.cpp
    src/snd_efx_reader.cpp
)

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

set(ALSOFT_EXAMPLES OFF)
set(ALSOFT_TESTS OFF)
set(ALSOFT_UTILS OFF)
add_subdirectory(externals/openal-soft)

set(ALURE_INSTALL OFF)
set(ALURE_BUILD_STATIC OFF)
set(ALURE_ENABLE_WAVE OFF)
set(ALURE_ENABLE_VORBIS OFF)
set(ALURE_ENABLE_FLAC OFF)
set(ALURE_ENABLE_SNDFILE ON)
set(SNDFILE_LIBRARY "C:\\Program Files (x86)\\Mega-Nerd\\libsndfile\\lib\\libsndfile-1.lib")
set(SNDFILE_INCLUDE_DIR "C:\\Program Files (x86)\\Mega-Nerd\\libsndfile\\include")
set(ALURE_BUILD_EXAMPLES OFF)
add_subdirectory(externals/alure)

add_subdirectory(externals/metahook)

target_link_libraries(MetaAudio PRIVATE OpenAL alure2)
