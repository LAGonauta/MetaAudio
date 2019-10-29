cmake_minimum_required(VERSION 3.8)

project(MetaHook)

set(SOURCES
    Launcher.rc
    src/commandline.cpp
    src/launcher.cpp
    src/sys_launcher.cpp
    src/LoadBlob.cpp
    src/metahook.cpp
    src/OEPHook.cpp
    src/registry.cpp
    src/detours/detours.cpp
    src/detours/disasm.cpp
    src/detours/image.cpp
    src/detours/modules.cpp
    src/common/interface.cpp
)

add_executable (metahook WIN32 ${SOURCES})

target_include_directories(metahook
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/detours
        ${PROJECT_SOURCE_DIR}/include/interface
)

target_include_directories(metahook
    PRIVATE
        ${PROJECT_SOURCE_DIR}/HLSDK/common
        ${PROJECT_SOURCE_DIR}/HLSDK/cl_dll
        ${PROJECT_SOURCE_DIR}/HLSDK/engine
        ${PROJECT_SOURCE_DIR}/HLSDK/public
        ${PROJECT_SOURCE_DIR}/HLSDK/pm_shared
)