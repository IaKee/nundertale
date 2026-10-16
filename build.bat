@echo off
setlocal enabledelayedexpansion

set TARGET=%1
if "%TARGET%"=="" set TARGET=standard

set CXX=.\vendor\w64devkit\bin\g++.exe
set INCLUDES=-Iinclude -Ivendor\raylib-6.2_win64_mingw-w64\include
set LIBDIRS=-Lvendor\raylib-6.2_win64_mingw-w64\lib
set LIBS=-lraylib -lopengl32 -lgdi32 -lwinmm

set SOURCES=
for /r src %%f in (*.cpp) do (
    set SOURCES=!SOURCES! "%%f"
)

if /i "%TARGET%"=="debug" goto :BUILD_DEBUG
if /i "%TARGET%"=="release" goto :BUILD_RELEASE
goto :BUILD_STANDARD

:BUILD_DEBUG
echo ========================================================
echo  Compiling nUndertale DEBUG Mode: -O0 -g -DNUNDERTALE_DEBUG
echo ========================================================
set CXXFLAGS=-std=c++20 -O0 -g -DNUNDERTALE_DEBUG -Wall -Wextra -Wno-unused-parameter
set OUTFILE=nundertale_modern.exe
goto :DO_COMPILE

:BUILD_RELEASE
echo ========================================================
echo  Compiling nUndertale RELEASE Mode: -O2 -s -mwindows
echo ========================================================
set CXXFLAGS=-std=c++20 -O2 -s -mwindows -Wall -Wextra -Wno-unused-parameter
set OUTFILE=nundertale_modern.exe
goto :DO_COMPILE

:BUILD_STANDARD
echo ========================================================
echo  Compiling nUndertale Standard Mode: C++20 + Raylib
echo ========================================================
set CXXFLAGS=-std=c++20 -O2 -DNUNDERTALE_DEBUG -Wall -Wextra -Wno-unused-parameter
set OUTFILE=nundertale_modern.exe
goto :DO_COMPILE

:DO_COMPILE
%CXX% %CXXFLAGS% %INCLUDES% !SOURCES! %LIBDIRS% %LIBS% -o %OUTFILE%

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================================
    echo  BUILD SUCCESSFUL: %OUTFILE%
    echo ========================================================
) else (
    echo.
    echo ========================================================
    echo  BUILD FAILED! Please check the errors above.
    echo ========================================================
    exit /b %ERRORLEVEL%
)
