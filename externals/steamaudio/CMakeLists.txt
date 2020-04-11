cmake_minimum_required(VERSION 3.1.0)

project(SteamAudio)

# Windows.
if(WIN32)
    # 64-bit.
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SteamAudio_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/Windows/x64/phonon.lib")
        set(SteamAudio_DLL "${CMAKE_CURRENT_SOURCE_DIR}/bin/Windows/x64/phonon.dll")
    # 32-bit.
    else()
        set(SteamAudio_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/Windows/x86/phonon.lib")
        set(SteamAudio_DLL "${CMAKE_CURRENT_SOURCE_DIR}/bin/Windows/x86/phonon.dll")
    endif()
    
    # Copy DLL to build directory.
    if(MSVC)
        configure_file(${SteamAudio_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/phonon.dll COPYONLY)
        configure_file(${SteamAudio_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/phonon.dll COPYONLY)
    else()
        configure_file(${SteamAudio_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/phonon.dll COPYONLY)
    endif()
# Mac.
elseif(APPLE)
    set(SteamAudio_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/OSX/libphonon.dylib")
# Android.
elseif(ANDROID)
    set(SteamAudio_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/Android/libphonon.so")
# Linux.
elseif(UNIX)
    # 64-bit.
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SteamAudio_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/Linux/x64/libphonon.so")
    # 32-bit.
    else()
        set(SteamAudio_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/Linux/x86/libphonon.so")
    endif()
endif()

# Library to link to.
add_library(SteamAudio INTERFACE)
target_include_directories(SteamAudio INTERFACE include)
target_link_libraries(SteamAudio INTERFACE ${SteamAudio_LIBRARY})
