@echo off

REM Create an empty output directory.
cd ..
rd /s /q Build
mkdir Build
cd Build

REM Generate a native project solution.
cmake -g "Visual Studio 12" ../Project

REM Prevent from automatically closing the console window.
pause
