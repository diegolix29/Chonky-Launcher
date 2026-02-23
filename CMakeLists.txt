cmake_minimum_required(VERSION 3.16)
project(ChonkyLauncher VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES
    main.cpp
    chonkylauncher.cpp
    chonkylauncher.h
)

# Create executable
add_executable(ChonkyLauncher ${SOURCES})

# Link Qt libraries
target_link_libraries(ChonkyLauncher Qt6::Core Qt6::Widgets)

# Release-specific settings
if(MSVC)
    set_property(TARGET ChonkyLauncher PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
    
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        # Enable Link Time Optimization (LTO)
        set_property(TARGET ChonkyLauncher PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
        
        # Strip debug symbols
        set_property(TARGET ChonkyLauncher PROPERTY LINK_FLAGS_RELEASE "/DEBUG /OPT:REF /OPT:ICF")
    endif()
endif()

# Set Windows subsystem to avoid console window
if(WIN32)
    set_target_properties(ChonkyLauncher PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Output directories
set_target_properties(ChonkyLauncher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>
)

# Qt deployment (Windows only)
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ChonkyLauncher POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:ChonkyLauncher>
            COMMENT "Deploying Qt libraries")
    endif()
endif()
