cmake_minimum_required(VERSION 3.20)
project(tarc-gui VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find Qt6 (richiede anche Core5Compat per alcuni componenti)
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)
find_package(Qt6Core5Compat REQUIRED)

# ────────────────────────────────────────────────────────────────────────────
# DIPENDENZE DEL CORE (FetchContent direttamente)
# ────────────────────────────────────────────────────────────────────────────
include(FetchContent)

# zstd
FetchContent_Declare(
    zstd
    GIT_REPOSITORY https://github.com/facebook/zstd.git
    GIT_TAG v1.5.6
    SOURCE_SUBDIR build/cmake
)
set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_STATIC ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(zstd)

# lz4
FetchContent_Declare(
    lz4
    GIT_REPOSITORY https://github.com/lz4/lz4.git
    GIT_TAG v1.10.0
    SOURCE_SUBDIR build/cmake
)
set(LZ4_BUILD_CLI OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(lz4)

# xz/lzma
FetchContent_Declare(
    liblzma
    GIT_REPOSITORY https://github.com/tukaani-project/xz.git
    GIT_TAG v5.6.3
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(XZ_BUILD_EXECUTABLES OFF CACHE BOOL "" FORCE)
set(ENABLE_NLS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(liblzma)

# xxhash
FetchContent_Declare(
    xxhash
    GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
    GIT_TAG v0.8.2
)
FetchContent_MakeAvailable(xxhash)

add_library(xxhash_lib STATIC ${xxhash_SOURCE_DIR}/xxhash.c)
target_include_directories(xxhash_lib PUBLIC ${xxhash_SOURCE_DIR})

# ────────────────────────────────────────────────────────────────────────────
# CORE TARC - Includi i file sorgente direttamente
# ────────────────────────────────────────────────────────────────────────────
set(CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/core/src/engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/src/io.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/src/license.cpp
)

# Crea la libreria statica del core
add_library(tarc_core STATIC ${CORE_SOURCES})

target_include_directories(tarc_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
    ${zstd_SOURCE_DIR}/lib
    ${lz4_SOURCE_DIR}/lib
    ${liblzma_SOURCE_DIR}/src/liblzma/api
    ${xxhash_SOURCE_DIR}
)

target_link_libraries(tarc_core PUBLIC
    libzstd_static
    lz4_static
    liblzma
    xxhash_lib
)

if(APPLE)
    target_compile_options(tarc_core PRIVATE -Wno-unused-variable)
endif()

# ────────────────────────────────────────────────────────────────────────────
# GUI SOURCES - Con gestione automatica MOC
# ────────────────────────────────────────────────────────────────────────────
set(GUI_HEADERS
    src/gui/mainwindow.h
    src/gui/archivemodel.h
    src/gui/workerthread.h
    src/gui/settingsdialog.h
)

set(GUI_SOURCES
    src/gui/main.cpp
    src/gui/mainwindow.cpp
    src/gui/archivemodel.cpp
    src/gui/workerthread.cpp
    src/gui/settingsdialog.cpp
)

# Qt resources
qt6_add_resources(GUI_RESOURCES
    src/gui/resources.qrc
)

# Executable - Qt6 automaticamente gestisce i MOC
qt6_add_executable(tarc-gui
    ${GUI_SOURCES}
    ${GUI_HEADERS}
    ${GUI_RESOURCES}
)

# Include directories per la GUI
target_include_directories(tarc-gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
)

# Link libraries
target_link_libraries(tarc-gui PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Core5Compat
    tarc_core
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(tarc-gui PRIVATE 
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
    )
    set_target_properties(tarc-gui PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(APPLE)
    target_compile_options(tarc-gui PRIVATE 
        -Wno-unused-variable
        -Wno-deprecated-declarations
    )
    set_target_properties(tarc-gui PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(tarc-gui PRIVATE
        pthread
        dl
    )
endif()

message(STATUS "")
message(STATUS "  ╔════════════════════════════════════════════╗")
message(STATUS "  ║        TARC GUI v${PROJECT_VERSION} - BUILD CONFIG      ║")
message(STATUS "  ╠════════════════════════════════════════════╣")
message(STATUS "  ║  Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "  ║  Compiler   : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  ║  Qt version : ${Qt6_VERSION}")
message(STATUS "  ╚════════════════════════════════════════════╝")
message(STATUS "")
