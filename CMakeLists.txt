cmake_minimum_required(VERSION 3.22)
project(GameOfLife)

set(CMAKE_CXX_STANDARD 23)

set(CLIP_EXAMPLES OFF CACHE BOOL "Compile clip examples")
set(CLIP_TESTS OFF CACHE BOOL "Compile clip tests")
add_subdirectory(clip)

add_subdirectory(tinyfiledialogs)

find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
        core/Core.h
        core/Core.cpp
        core/CoreExtension.cpp
        core/CoreExtension.h
        Component.cpp
        Component.h
        Window.cpp
        Window.h
        core/CoreRenderer.cpp
        core/CoreRenderer.h
        core/CoreSelector.cpp
        core/CoreSelector.h
        Game.cpp
        Game.h
        ui/GPS.cpp
        ui/GPS.h
        ui/Character.cpp
        ui/Character.h
        ui/characterSet.h
        ui/Font.cpp
        ui/Font.h
        RectBuffer.cpp
        RectBuffer.h
        ui/Settings.cpp
        ui/Settings.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static clip tinyfiledialogs -Ofast)
    target_compile_options(${PROJECT_NAME} PRIVATE -Ofast)
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static clip tinyfiledialogs)
endif ()
