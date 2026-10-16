cmake_minimum_required(VERSION 3.16)
project(nundertale LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    include
    vendor/raylib-6.2_win64_mingw-w64/include
)

# Link directories
link_directories(
    vendor/raylib-6.2_win64_mingw-w64/lib
)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Executable target
add_executable(nundertale ${SOURCES})

# Target libraries
target_link_libraries(nundertale PRIVATE
    raylib
    opengl32
    gdi32
    winmm
)
