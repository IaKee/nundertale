@echo off
setlocal

call .\lint.bat
if errorlevel 1 exit /b %ERRORLEVEL%

set CXX=.\vendor\w64devkit\bin\g++.exe
set CXXFLAGS=-std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Iinclude -Ivendor\raylib-6.2_win64_mingw-w64\include

%CXX% %CXXFLAGS% tests\test_runner.cpp -o tests\test_runner.exe
if errorlevel 1 exit /b %ERRORLEVEL%

tests\test_runner.exe
exit /b %ERRORLEVEL%
