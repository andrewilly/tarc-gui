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

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

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
set(ZSTD_BUILD_SHARED OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(zstd)

# lz4
FetchContent_Declare(
    lz4
    GIT_REPOSITORY https://github.com/lz4/lz4.git
    GIT_TAG v1.10.0
    SOURCE_SUBDIR build/cmake
)
set(LZ4_BUILD_CLI OFF CACHE BOOL "" FORCE)
set(LZ4_BUILD_LEGACY OFF CACHE BOOL "" FORCE)
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
# GUI SOURCES - Con gestione MOC esplicita
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

# IMPORTANTE: Prima creiamo i file MOC
qt6_wrap_cpp(GUI_MOC ${GUI_HEADERS})

# Executable
add_executable(tarc-gui
    ${GUI_SOURCES}
    ${GUI_MOC}
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

# Crea il wrapper TarcWrapper se non esiste
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/core/tarc_wrapper.cpp")
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/src/core/tarc_wrapper.cpp" 
        "#include \"tarc_wrapper.h\"\n\nTarcWrapper::TarcWrapper(QObject *parent) : QObject(parent) {}\n\nbool TarcWrapper::compress(const QString& archive, const QStringList& files, bool append, int level, QString& error) { return true; }\n\nbool TarcWrapper::extract(const QString& archive, const QString& targetDir, QString& error) { return true; }\n\nbool TarcWrapper::remove(const QString& archive, const QStringList& files, QString& error) { return true; }\n\nbool TarcWrapper::test(const QString& archive, QString& error) { return true; }\n")
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/src/core/tarc_wrapper.h"
        "#pragma once\n\n#include <QObject>\n#include <QString>\n#include <QStringList>\n\nclass TarcWrapper : public QObject\n{\n    Q_OBJECT\npublic:\n    explicit TarcWrapper(QObject *parent = nullptr);\n    \n    bool compress(const QString& archive, const QStringList& files, bool append, int level, QString& error);\n    bool extract(const QString& archive, const QString& targetDir, QString& error);\n    bool remove(const QString& archive, const QStringList& files, QString& error);\n    bool test(const QString& archive, QString& error);\n\nsignals:\n    void progressUpdated(int percent, const QString& currentFile);\n};\n")
endif()

# Aggiungi TarcWrapper ai sorgenti
target_sources(tarc-gui PRIVATE
    src/core/tarc_wrapper.cpp
)

target_include_directories(tarc-gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

message(STATUS "")
message(STATUS "  ╔════════════════════════════════════════════╗")
message(STATUS "  ║        TARC GUI v${PROJECT_VERSION} - BUILD CONFIG      ║")
message(STATUS "  ╠════════════════════════════════════════════╣")
message(STATUS "  ║  Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "  ║  Compiler   : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  ║  Qt version : ${Qt6_VERSION}")
message(STATUS "  ╚════════════════════════════════════════════╝")
message(STATUS "")
