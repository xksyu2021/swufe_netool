cmake_minimum_required(VERSION 4.3)
project(swufe_netool)

set(CMAKE_CXX_STANDARD 23)

add_executable(swufe_netool WIN32
        gui/app.manifest
        gui/main.cpp
        gui/share.h
        gui/window.cpp
        gui/ipc.cpp
        gui/toolkit/font.cpp
        gui/toolkit/creater.cpp
)

target_compile_definitions(swufe_netool PRIVATE
        WIN32_LEAN_AND_MEAN
        UNICODE
        _UNICODE
        NOMINMAX
)

add_compile_definitions(
        WINVER=0x0A00
        _WIN32_WINNT=0x0A00
        NTDDI_VERSION=0x0A000006
)

if(MSVC)
    target_compile_options(swufe_netool PRIVATE /utf-8)
endif()