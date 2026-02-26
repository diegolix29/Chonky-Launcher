cmake_minimum_required(VERSION 3.16)
project(ChonkyLauncher VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# =========================
# Qt6
# =========================
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# =========================
# SDL3 (ALWAYS download)
# =========================
include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.0   # pin this for reproducible builds
)

# SDL options
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

# =========================
# Sources
# =========================
set(SOURCES
    main.cpp
    chonkylauncher.cpp
    chonkylauncher.h
)

add_executable(ChonkyLauncher ${SOURCES})

# Enable SDL3 Gamepad API globally
target_compile_definitions(ChonkyLauncher PRIVATE SDL_ENABLE_GAMEPAD)

# =========================
# Link libraries
# =========================
target_link_libraries(ChonkyLauncher
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        SDL3::SDL3
)

# =========================
# MSVC tweaks
# =========================
if(MSVC)
    set_property(TARGET ChonkyLauncher
        PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
        ${CMAKE_BINARY_DIR}/$<CONFIGURATION>
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set_property(TARGET ChonkyLauncher
            PROPERTY INTERPROCEDURAL_OPTIMIZATION ON
        )
    endif()
endif()

# =========================
# Windows subsystem (no console)
# =========================
if(WIN32)
    set_target_properties(ChonkyLauncher PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# =========================
# Output directories
# =========================
set_target_properties(ChonkyLauncher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>
)

# =========================
# Qt deployment (Windows)
# =========================
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ChonkyLauncher POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:ChonkyLauncher>
            COMMENT "Deploying Qt libraries"
        )
    endif()
endif()

# =========================
# Copy SDL3 runtime (Windows)
# =========================
if(WIN32)
    add_custom_command(TARGET ChonkyLauncher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:SDL3::SDL3>"
            "$<TARGET_FILE_DIR:ChonkyLauncher>"
        COMMENT "Copying SDL3.dll"
    )
endif()