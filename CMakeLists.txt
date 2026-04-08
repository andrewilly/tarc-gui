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
# TARC CORE (submodule)
# ────────────────────────────────────────────────────────────────────────────
# Verifica se il submodule esiste
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/core/CMakeLists.txt")
    add_subdirectory(core)
    
    # Il core produce un eseguibile chiamato "tarc"
    # Creiamo un alias per la libreria
    if(NOT TARGET tarc_core)
        # Crea un target INTERFACE per linkare i file del core
        add_library(tarc_core INTERFACE)
        target_include_directories(tarc_core INTERFACE 
            ${CMAKE_CURRENT_SOURCE_DIR}/core/include
        )
        target_link_libraries(tarc_core INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/core/src/engine.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/core/src/io.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/core/src/license.cpp
        )
        # Link dipendenze del core
        target_link_libraries(tarc_core INTERFACE
            libzstd_static
            lz4_static
            liblzma
            xxhash_lib
        )
    endif()
else()
    message(WARNING "Core submodule not found! Please run: git submodule update --init --recursive")
endif()

# ────────────────────────────────────────────────────────────────────────────
# GUI SOURCES
# ────────────────────────────────────────────────────────────────────────────
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

# Executable
qt6_add_executable(tarc-gui
    ${GUI_SOURCES}
    ${GUI_RESOURCES}
)

# Include directories
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
    )
    set_target_properties(tarc-gui PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()

# Linux
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
