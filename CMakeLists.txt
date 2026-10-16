cmake_minimum_required(VERSION 3.16)
project(nundertale LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    include
    vendor/raylib-6.2_win64_mingw-w64/include
)

link_directories(
    vendor/raylib-6.2_win64_mingw-w64/lib
)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(nundertale_modern ${SOURCES})

target_compile_definitions(nundertale_modern PRIVATE
    $<$<CONFIG:Debug>:NUNDERTALE_DEBUG>
)

target_link_libraries(nundertale_modern PRIVATE
    raylib
    opengl32
    gdi32
    winmm
)
