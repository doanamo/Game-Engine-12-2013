@echo off

cd ..
rd /s /q Build
mkdir Build
cd Build

cmake -g "Visual Studio 12" ../Project

pause
