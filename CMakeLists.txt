cmake_minimum_required(VERSION 3.16)
project(ChonkyLauncher VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_TOOLCHAIN_FILE "C:/Users/Diego Mugetsu/source/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    add_compile_options(/utf-8)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES
    main.cpp
    chonkylauncher.cpp
    chonkylauncher.h
)

add_executable(ChonkyLauncher ${SOURCES})

target_link_libraries(ChonkyLauncher Qt6::Core Qt6::Widgets)

if(MSVC)
    set_property(TARGET ChonkyLauncher PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
endif()

if(WIN32)
    set_target_properties(ChonkyLauncher PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ChonkyLauncher POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:ChonkyLauncher>
            COMMENT "Deploying Qt libraries")
    endif()
endif()
