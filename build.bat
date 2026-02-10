@echo off
setlocal

echo ========================================================
echo  Compiling nUndertale (C++20 + Raylib - SOLID Architecture)
echo ========================================================

set CXX=.\vendor\w64devkit\bin\g++.exe
set INCLUDES=-Iinclude -Ivendor\raylib-6.2_win64_mingw-w64\include
set LIBDIRS=-Lvendor\raylib-6.2_win64_mingw-w64\lib
set LIBS=-lraylib -lopengl32 -lgdi32 -lwinmm
set CXXFLAGS=-std=c++20 -O2 -Wall -Wextra -Wno-unused-parameter

set SOURCES=src\main.cpp ^
            src\core\Game.cpp ^
            src\core\SceneManager.cpp ^
            src\services\AudioService.cpp ^
            src\services\ResourceService.cpp ^
            src\services\InputService.cpp ^
            src\entities\Player.cpp ^
            src\entities\Interactable.cpp ^
            src\ui\DialogBox.cpp ^
            src\combat\BattleSystem.cpp ^
            src\scenes\DisclaimerScene.cpp ^
            src\scenes\LoreScene.cpp ^
            src\scenes\TitleScene.cpp ^
            src\scenes\InstructionsScene.cpp ^
            src\scenes\NameSelectScene.cpp ^
            src\scenes\OverworldScene.cpp ^
            src\scenes\BattleScene.cpp

%CXX% %CXXFLAGS% %INCLUDES% %SOURCES% %LIBDIRS% %LIBS% -o nundertale_modern.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================================
    echo  BUILD SUCCESSFUL! Generated: nundertale_modern.exe
    echo ========================================================
) else (
    echo.
    echo ========================================================
    echo  BUILD FAILED! Please check the errors above.
    echo ========================================================
    exit /b %ERRORLEVEL%
)
