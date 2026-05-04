@echo off
setlocal

set CXX=.\vendor\w64devkit\bin\g++.exe
set CXXFLAGS=-std=c++20 -O2 -Wall -Wextra -Wpedantic -Wshadow -Wconversion

%CXX% %CXXFLAGS% -Iinclude tools\content_lint.cpp -o tools\content_lint.exe
if errorlevel 1 exit /b 2

tools\content_lint.exe .
exit /b %ERRORLEVEL%
