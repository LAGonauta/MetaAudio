cmake_minimum_required(VERSION 3.27)

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

# cleanup libSndFile testing configuration
option(BUILD_EXAMPLES "" OFF)
option(BUILD_PROGRAMS "" OFF)
option(BUILD_TESTING "" OFF)
option(INSTALL_MANPAGES "" OFF)

# Collect all source files
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
    src/Config/EfxJsonReader.cpp
    src/Config/SettingsManager.cpp
    src/Effects/GoldSrcOcclusionCalculator.cpp
    src/Effects/SteamAudioOcclusionCalculator.cpp
    src/Utilities/Fade.cpp
    src/Workarounds/NoWorkarounds.cpp
    src/Workarounds/XFiWorkarounds.cpp
    src/Loaders/GoldSrcFileBuf.cpp
    src/Loaders/GoldSrcFileStream.cpp
    src/Loaders/GoldSrcFileFactory.cpp
    src/Loaders/SoundLoader.cpp
    src/Loaders/SoxrDecoder.cpp
    src/Loaders/SoxrBitDepthNormalizerDecoder.cpp
    src/AudioEngine.cpp
    src/Utilities/ChannelManager.cpp
    src/Utilities/SoxrResamplerHelper.cpp
    src/snd_local.cpp
    src/SoundSources/SoundSourceFactory.cpp
    src/SoundSources/BaseSoundSource.cpp
    src/SoundSources/StaticSoundSource.cpp
    src/SoundSources/StreamingSoundSource.cpp
    # Use interface.cpp from HLSDK
    ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/include/HLSDK/common/interface.cpp
)

# Collect all header files from include directory
file(GLOB_RECURSE HEADERS 
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
)

add_library(MetaAudio SHARED ${SOURCES} ${HEADERS})

set_property(TARGET MetaAudio PROPERTY CXX_STANDARD 17)

# Copy MSVC configuration files to build directory for Visual Studio projects
if(MSVC)
    set(MSVC_CONFIG_FILES
        "${PROJECT_SOURCE_DIR}/msvc/Directory.build.props"
        "${PROJECT_SOURCE_DIR}/msvc/Directory.build.targets"
        "${PROJECT_SOURCE_DIR}/msvc/MetaHookSv.props"
    )
    foreach(CONFIG_FILE ${MSVC_CONFIG_FILES})
        get_filename_component(FILE_NAME ${CONFIG_FILE} NAME)
        configure_file(${CONFIG_FILE} "${CMAKE_BINARY_DIR}/${FILE_NAME}" COPYONLY)
    endforeach()
endif()

target_compile_definitions(MetaAudio PRIVATE AUDIO_EXPORTS)
add_compile_definitions(NOMINMAX PICOJSON_USE_INT64)

target_include_directories(MetaAudio
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/externals/picojson
        ${PROJECT_SOURCE_DIR}/externals/steamaudio/include
        ${PROJECT_SOURCE_DIR}/externals/soxr/src
        ${PROJECT_SOURCE_DIR}/externals/alure/include/AL
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/include
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/include/Interface
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/include/HLSDK/common
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/include/HLSDK/cl_dll
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/include/HLSDK/engine
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/include/HLSDK/public
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/include/HLSDK/pm_shared
        ${PROJECT_SOURCE_DIR}/externals/MetaHookSv/thirdparty/capstone_fork/include/capstone
)

# MetaAudio requires OpenAL
# Configure OpenAL-Soft options before adding subdirectory
option(ALSOFT_UTILS "Build utility programs" OFF)
option(ALSOFT_NO_CONFIG_UTIL "Disable building the alsoft-config utility" ON)
option(ALSOFT_EXAMPLES "Build example programs" OFF)
option(ALSOFT_TESTS "Build test programs" OFF)
option(ALSOFT_INSTALL "Install main library" OFF)
option(ALSOFT_INSTALL_CONFIG "Install alsoft.conf sample configuration file" OFF)
option(ALSOFT_INSTALL_HRTF_DATA "Install HRTF data files" OFF)
option(ALSOFT_INSTALL_AMBDEC_PRESETS "Install AmbDec preset files" OFF)
option(ALSOFT_INSTALL_EXAMPLES "Install example programs" OFF)

# Add OpenAL-Soft as subdirectory
add_subdirectory(externals/openal-soft)

# This fixes OpenAL not found for alure2 in some cases
export(TARGETS OpenAL
    NAMESPACE OpenAL::
    FILE "${CMAKE_BINARY_DIR}/OpenALConfig.cmake"
)

# Set OpenAL variables for Alure's find_package to succeed
# openal-soft creates a target 'OpenAL' that we can reference
set(OPENAL_LIBRARY OpenAL)
set(OPENAL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/externals/openal-soft/include)
set(OPENAL_FOUND TRUE)

# libSndFile requires Ogg, Vorbis, FLAC, Opus for external codec support
# Configure Ogg options before adding subdirectory
option(INSTALL_DOCS "Install Ogg documentation" OFF)
option(INSTALL_PKG_CONFIG_MODULE "Install Ogg pkg-config module" OFF)
option(INSTALL_CMAKE_PACKAGE_MODULE "Install Ogg CMake package configuration module" ON)

# Add Ogg as subdirectory (build as static library)
add_subdirectory(externals/ogg)

# Set Ogg variables for dependent libraries' find_package to succeed
# Ogg creates a target 'ogg' and alias 'Ogg::ogg' that we can reference
set(OGG_LIBRARY ogg)
set(OGG_LIBRARIES ogg)
set(OGG_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/externals/ogg/include)
set(OGG_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/externals/ogg/include)
set(Ogg_FOUND TRUE)
set(OGG_FOUND TRUE)

# Configure FLAC options before adding subdirectory
option(BUILD_CXXLIBS "Build libFLAC++" OFF)
option(BUILD_PROGRAMS "Build and install FLAC programs" OFF)
option(BUILD_DOCS "Build and install FLAC doxygen documents" OFF)
option(INSTALL_MANPAGES "Install FLAC MAN pages" OFF)
option(INSTALL_PKGCONFIG_MODULES "Install FLAC PkgConfig modules" OFF)
option(INSTALL_CMAKE_CONFIG_MODULE "Install FLAC CMake package-config module" ON)
option(WITH_OGG "FLAC ogg support" ON)

# Add FLAC as subdirectory (build as static library)
add_subdirectory(externals/flac)

# Set FLAC variables for libSndFile's find_package to succeed
# FLAC creates a target 'FLAC' and alias 'FLAC::FLAC' that we can reference
set(FLAC_LIBRARY FLAC)
set(FLAC_LIBRARIES FLAC)
set(FLAC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/externals/flac/include)
set(FLAC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/externals/flac/include)
set(FLAC_FOUND TRUE)

# Configure Vorbis options before adding subdirectory
option(INSTALL_CMAKE_PACKAGE_MODULE "Install Vorbis CMake package configuration module" ON)

# Add Vorbis as subdirectory (build as static library)
add_subdirectory(externals/vorbis)

# Set Vorbis variables for libSndFile's find_package to succeed
# Vorbis creates targets 'vorbis', 'vorbisenc', 'vorbisfile' and aliases 'Vorbis::*'
set(VORBIS_LIBRARY vorbis)
set(VORBIS_LIBRARIES vorbis)
set(VORBISENC_LIBRARY vorbisenc)
set(VORBISENC_LIBRARIES vorbisenc)
set(VORBISFILE_LIBRARY vorbisfile)
set(VORBISFILE_LIBRARIES vorbisfile)
set(VORBIS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/externals/vorbis/include)
set(VORBIS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/externals/vorbis/include)
set(Vorbis_FOUND TRUE)
set(VORBIS_FOUND TRUE)

# Configure Opus options before adding subdirectory
option(OPUS_BUILD_SHARED_LIBRARY "Build Opus shared library" OFF)
option(OPUS_BUILD_TESTING "Build Opus tests" OFF)
option(OPUS_BUILD_PROGRAMS "Build Opus programs" OFF)
option(OPUS_INSTALL_PKG_CONFIG_MODULE "Install Opus pkg-config module" OFF)
option(OPUS_INSTALL_CMAKE_CONFIG_MODULE "Install Opus CMake package config module" ON)

# Add Opus as subdirectory (build as static library)
add_subdirectory(externals/opus)

# Set Opus variables for libSndFile's find_package to succeed
# Opus creates a target 'opus' and alias 'Opus::opus' that we can reference
set(OPUS_LIBRARY opus)
set(OPUS_LIBRARIES opus)
set(OPUS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/externals/opus/include)
set(OPUS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/externals/opus/include)
set(Opus_FOUND TRUE)
set(OPUS_FOUND TRUE)

# Opus include directory is already set by the opus CMakeLists.txt
# No need to modify it here

option(ENABLE_EXTERNAL_LIBS "Enable external libraries" ON)

# Alure requires libSndFile
# Configure libsndfile to build as shared library
set(BUILD_SHARED_LIBS ON)
add_subdirectory(externals/libsndfile)
# Reset BUILD_SHARED_LIBS to avoid affecting other dependencies
set(BUILD_SHARED_LIBS OFF)

# Create opus subdirectory structure for libsndfile compatibility
# libsndfile expects opus headers in opus/ subdirectory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/opus_headers/opus)

# Copy Opus headers to opus subdirectory for libsndfile compatibility
file(COPY ${PROJECT_SOURCE_DIR}/externals/opus/include/
     DESTINATION ${CMAKE_BINARY_DIR}/opus_headers/opus/
     FILES_MATCHING PATTERN "*.h")

# Add the opus headers directory to sndfile target for compilation
target_include_directories(sndfile PRIVATE ${CMAKE_BINARY_DIR}/opus_headers)

# Set SndFile variables for Alure's find_package to succeed
# libsndfile creates a target 'sndfile' that we can reference
set(SNDFILE_LIBRARY sndfile)
set(SNDFILE_LIBRARIES sndfile)
set(SNDFILE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/externals/libsndfile/include)
set(SNDFILE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/externals/libsndfile/include)
set(SNDFILE_FOUND TRUE)

# MetaAudio requires Alure
add_subdirectory(externals/alure)

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
    # Force C++17 as mpark/variant doesn't compile with MSVC
    set_property(TARGET ${ALURE2_LIBRARY} PROPERTY CXX_STANDARD 17)
    target_compile_options(${ALURE2_LIBRARY} PUBLIC "/Zc:__cplusplus")
    target_compile_options(MetaAudio PUBLIC "/Zc:__cplusplus")
endif()

option(BUILD_TESTS OFF)
option(WITH_LSR_BINDINGS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Externals/soxr/cmake/Modules)
add_subdirectory(externals/soxr)

# Ensure proper build order dependencies
# FLAC and Vorbis depend on Ogg
add_dependencies(FLAC ogg)
add_dependencies(vorbis ogg)
add_dependencies(vorbisenc vorbis)
add_dependencies(vorbisfile vorbis)
# sndfile depends on all codec libraries
add_dependencies(sndfile ogg FLAC vorbis vorbisenc vorbisfile opus)
# Alure depends on OpenAL and sndfile
add_dependencies(${ALURE2_LIBRARY} OpenAL sndfile)
# MetaAudio depends on Alure
add_dependencies(MetaAudio ${ALURE2_LIBRARY})

target_link_libraries(MetaAudio PRIVATE OpenAL ${ALURE2_LIBRARY} sndfile soxr)

# Ensure all libraries use the same MSVC runtime library as MetaAudio
if(MSVC)
    set_property(TARGET sndfile PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET ogg PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET FLAC PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET vorbis PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET vorbisenc PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET vorbisfile PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET opus PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET OpenAL PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET ${ALURE2_LIBRARY} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Install MetaAudio.dll, libsndfile.dll and OpenAL.dll to bin directory
install(TARGETS MetaAudio OpenAL sndfile
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
